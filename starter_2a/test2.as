apple   lw      0       1       five    load reg1 with 5 (symbolic address) - note that this instruction is at address 0
        lw      1       2       3005       load reg2 with -1 (numeric address)
        lw      1       7       apple       load reg2 with -1 (numeric address)
start   sw     1       2       neg1       decrement reg1
        noop
        sw     0       1       2       goto end of program when reg1==0
        sw     0       4       neg1       goto end of program when reg1==0
        noop
        halt                            end of program
five    .fill   5
neg1    .fill   -1
Ark     .fill   Ark


