    ;; fibo(30)
    30
    fibo rel call
    ;; pop arguments and push return value to stack
    pop rv
    ;; call print_number (syscall number is 1)
    1 syscall
    exit
fibo: ;; function fibo(x)
    ;; if (x > 2) then
    0 ldarg 2 gt
    .else rel jz
    ;; return fibo(x - 2) + fibo(x - 1)
    0 ldarg 1 sub
    fibo rel call
    pop rv
    0 ldarg 2 sub
    fibo rel call
    pop rv add
    srv
    ret
.else:
    ;; else return 1
    1 srv
    ret
