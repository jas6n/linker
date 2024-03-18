/**
 * Project 2
 * LC-2K Linker
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXSIZE 500
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
	char label[7];
	char location;
	unsigned int offset;
};

struct RelocationTableEntry {
	unsigned int offset;
	char inst[7];
	char label[7];
	unsigned int file;
};

struct FileData {
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	unsigned int textStartingLine; // in final executable
	unsigned int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	unsigned int textSize;
	unsigned int dataSize;
	unsigned int symbolTableSize;
	unsigned int relocationTableSize;
	int text[MAXSIZE*MAXFILES];
	int data[MAXSIZE*MAXFILES];
	SymbolTableEntry symbolTable[MAXSIZE*MAXFILES];
	RelocationTableEntry relocTable[MAXSIZE*MAXFILES];
};

unsigned int get_local_text(FileData f, unsigned int offset){
	return f.textStartingLine + offset;
}

unsigned int get_local_data(FileData f, unsigned int offset){
	return f.textStartingLine + offset;
}

// int relocate(int val, int n){

// 	int mask = 0b1111111110000000000000000;
// 	val = val & mask;
// 	val = val | n;
// 	return val;

// }

int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr; 
	unsigned int i, j;

	if (argc <= 2) {
		printf("error: usage: %s <obj file> ... <output-exe-file>\n",
				argv[0]);
		exit(1);
	}

	outFileString = argv[argc - 1];

	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	FileData files[MAXFILES];

  // read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; i++) {
		inFileString = argv[i+1];

		inFilePtr = fopen(inFileString, "r");
		printf("opening %s\n", inFileString);

		if (inFilePtr == NULL) {
			printf("error in opening %s\n", inFileString);
			exit(1);
		}

		char line[MAXLINELENGTH];
		unsigned int textSize, dataSize, symbolTableSize, relocationTableSize;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
				&textSize, &dataSize, &symbolTableSize, &relocationTableSize);

		files[i].textSize = textSize;
		files[i].dataSize = dataSize;
		files[i].symbolTableSize = symbolTableSize;
		files[i].relocationTableSize = relocationTableSize;

		// read in text section
		int instr;
		for (j = 0; j < textSize; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = strtol(line, NULL, 0);
			files[i].text[j] = instr;
		}

		// read in data section
		int data;
		for (j = 0; j < dataSize; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = strtol(line, NULL, 0);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		unsigned int addr;
		for (j = 0; j < symbolTableSize; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
					label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < relocationTableSize; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
					&addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file	= i;
		}
		fclose(inFilePtr);
	} // end reading files


	// *** INSERT YOUR CODE BELOW ***
	//    Happy coding!!!

	// check for errors
	// check for duplicate labels and undefined globals
	for (unsigned int i = 0; i < argc - 2; ++i){
		// looping through each symbol table entry
		for (unsigned int j = 0; j < files[i].symbolTableSize; ++j){
			unsigned int count1 = 0;
			unsigned int count2 = 0;
			if(!strcmp(files[i].symbolTable[j].label, "Stack")){
				if (files[i].symbolTable[j].location != 'U'){
					printf("%s", "cant define stack");
					exit(1);
				}
			}
			for (unsigned int k = 0; k < argc - 2; ++k){
				for (unsigned int l = 0; l < files[k].symbolTableSize; ++l){
					// if labels are equal
					if (!strcmp(files[i].symbolTable[j].label, files[k].symbolTable[l].label)){
						// if location isn't U for both
						if (files[i].symbolTable[j].location != 'U' && files[k].symbolTable[l].location != 'U'){
							count1 += 1;
						}

						if (files[k].symbolTable[l].location != 'U'){
							count2 += 1;
						}

					}
				}
			}
			if (count1 > 1){
				printf("%s", "no duplicates");
				exit(1);
			} 
			if (count2 < 1){
				printf("%s", "define labels");
				exit(1);
			} 
		}
	}


	// for (unsigned int i = 0; i < argc - 2; ++i){
	// 	for (unsigned int j = 0; j < files[i].symbolTableSize; ++j){

	// 	}
	// }

	unsigned int line = 0;


	// update where the files begin and end in reference to the final linker file
	for (unsigned int i = 0; i < argc - 2; ++i){
		files[i].textStartingLine = line;
		line += files[i].textSize;
	}


	for (unsigned int i = 0; i < argc - 2; ++i){
		files[i].dataStartingLine = line;
		line += files[i].dataSize;
	}


	// loop through and edit stuff in relocation table
	for (unsigned int i = 0; i < argc - 2; ++i){
		for (unsigned int j = 0; j < files[i].relocationTableSize; ++j){
			// unsigned int tstart = files[i].textStartingLine;
			unsigned int dstart = files[i].dataStartingLine;
			unsigned int offset = files[i].relocTable[j].offset;


			// if it is lw and local
			if (!strcmp(files[i].relocTable[j].inst, "lw") || !strcmp(files[i].relocTable[j].inst, "sw")){
				if (!isupper(files[i].relocTable[j].label[0])){

					unsigned int mask = 0b1111111110000000000000000;
					bool in_text = true;

					unsigned int temp = files[i].text[offset] & mask;

					unsigned int true_off = files[i].text[offset] - (temp);

					if (true_off >= files[i].textSize){
						in_text = false;
					}


					if (in_text){
						files[i].text[offset] += files[i].textStartingLine;
					}
					// handles if it is in data section
					else {
						true_off -= files[i].textSize;

						files[i].text[offset] = temp + true_off + dstart;
					}			
				} else if (!strcmp(files[i].relocTable[j].label, "Stack")){

				} else {

					char section = '\0';
					unsigned int val = 0;
					unsigned int f = 0;


					// loop thru symbol table to find section and val
					for (unsigned int a = 0; a < argc - 2; ++a){
						for (unsigned int b = 0; b < files[a].symbolTableSize; ++b){

							if (!strcmp(files[a].symbolTable[b].label, files[i].relocTable[j].label)){
								if (files[a].symbolTable[b].location != 'U'){
									section = files[a].symbolTable[b].location;
									val = files[a].symbolTable[b].offset;
									f = a;
								}
							}

						}
					}

					// find the true offset

					unsigned int off = 0;

					if (section == 'D'){
						off = files[f].dataStartingLine + val;
					} else {
						off = files[f].textStartingLine + val;
					}

					// update the machine code
					files[i].text[offset] += off;

				}
			}

		
			// if it is .fill and local
			// .fill would be similar 
			else if (!strcmp(files[i].relocTable[j].inst, ".fill")){
				if (!isupper(files[i].relocTable[j].inst[0])){

					
					bool in_text = true;

					unsigned int ind_orig_in_data = files[i].relocTable[j].offset;

					if (files[i].data[ind_orig_in_data] >= files[i].textSize){
						in_text = false;
					}


					if (in_text){				
						files[i].data[ind_orig_in_data] += files[i].textStartingLine;	
					} else {
						files[i].data[ind_orig_in_data] += files[i].dataStartingLine;
					}		
					
				} else if (!strcmp(files[i].relocTable[j].label, "Stack")){

				} else {

					char section = '\0';
					unsigned int val = 0;
					unsigned int f = 0;


					// loop thru symbol table to find section and val
					for (unsigned int a = 0; a < argc - 2; ++a){
						for (unsigned int b = 0; b < files[a].symbolTableSize; ++b){

							if (!strcmp(files[a].symbolTable[b].label, files[i].relocTable[j].label)){
								if (files[a].symbolTable[b].location != 'U'){
									section = files[a].symbolTable[b].location;
									val = files[a].symbolTable[b].offset;
									f = a;
								}
							}

						}
					}


					// find true offset
					unsigned int off = 0;

					if (section == 'D'){
						off = files[f].dataStartingLine + val;
					} else {
						off = files[f].textStartingLine + val;
					}

					// update the machine code
					files[i].data[offset] += off;


				}
			
			}
		}
	}


	// merge into complete file

	CombinedFiles complete;

	complete.textSize = 0;
	complete.dataSize = 0;
	complete.relocationTableSize = 0;
	complete.symbolTableSize = 0;

		for (unsigned int i = 0; i < argc - 2; ++i){

			for (unsigned int j = 0; j < files[i].textSize; ++j){
					complete.text[complete.textSize + j] = files[i].text[j];
					complete.text[complete.textSize + j] = files[i].text[j];
				}
				
			for (unsigned int j = 0; j < files[i].dataSize; ++j){
				complete.data[complete.dataSize + j] = files[i].data[j];
			}

			for (unsigned int j = 0; j < files[i].symbolTableSize; ++j){
				complete.symbolTable[complete.symbolTableSize + j] = files[i].symbolTable[j];
			}


			// edit the offsets here
			for (unsigned int j = 0; j < files[i].relocationTableSize; ++j){
				complete.relocTable[complete.relocationTableSize + j] = files[i].relocTable[j];
			}

		
			complete.textSize += files[i].textSize;
			complete.dataSize += files[i].dataSize;
			complete.relocationTableSize += files[i].relocationTableSize;
			complete.symbolTableSize += files[i].symbolTableSize;


	}

	// print out into file

	for (int i = 0; i < complete.textSize; ++i){
		int j = complete.text[i];
		fprintf(outFilePtr, "%d", j);
		fprintf(outFilePtr, "\n");
	}
	for (int i = 0; i < complete.dataSize; ++i){
		int j = complete.data[i];
		fprintf(outFilePtr, "%d", j);
		fprintf(outFilePtr, "\n");
	}

return 0;
} // main

