    beq     1   2   3
    jalr    5   6
stork lw    2   3   4
    noop
    halt
    lw     3    4   stork
    sw      2   3   5
skate  .fill   4  
    .fill   stork
    .fill   skate
