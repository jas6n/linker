start   lw     0       1       Ork
        jalr    6       7       
        beq     5       3       end 
        noop
end     nor     0       1       2 
apple   noop
Zee      halt
        .fill    -9
        .fill    100