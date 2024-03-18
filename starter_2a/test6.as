begin noop
      beq 0 0 -2
      lw  1 0 -3
      sw  1 2 -3
      beq 0 0 begin
      lw  1 1 10
      sw  2 2 10
      lw  1 0 Lii
      sw  1 3 Lii
      beq 7 2 1
      .fill 32767
lower .fill -32768