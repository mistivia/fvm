    ;; fibo(30)
    imm 30
    fibo rel call
    ;; pop arguments and push return value to stack
    pop rv
    ;; call print_number (syscall number is 1)
    imm 1 syscall
    exit
fibo:
    ;; setup stack pointer and base pointer
    bp sp sbp
    ;; get first argument
    imm 2 bpick
    ;; if (x > 2) then
    imm 2 gt
    .else rel jz
    ;; return fibo(x - 2) + fibo(x - 1)
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
    ;; else return 1
    imm 1 srv
    .end rel jmp
.end:
    bp ssp sbp
    ret
