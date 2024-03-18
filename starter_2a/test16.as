b   beq     1       1       100  
        beq     5       2      s
        noop
        jalr 1 2
s    nor     0       1       2
a   halt
n      noop
        .fill    0
        .fill    -280