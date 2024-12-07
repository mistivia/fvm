    imm 40
    fibo rel call
    pop rv
    imm 1 syscall
    exit
fibo:
    bp sp sbp
    imm 2 bpick
    imm 2 gt
    .else rel jz
    imm 2 bpick
    imm 1 sub
    fibo rel call
    pop rv
    imm 2 bpick
    imm 2 sub
    fibo rel call
    pop rv add
    srv
    .end rel jmp
.else:
    imm 1 srv
    .end rel jmp
.end:
    bp ssp sbp
    ret
