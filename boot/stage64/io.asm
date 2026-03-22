section .text
global printf
printf:
    push rbp
    mov rbp, rsp

    ;; Push rdi, rsi, rdx, rcx, r8, r9
    push r9
    push r8
    push rcx
    push rdx
    push rsi 
    push rdi

    mov rdi, rsp
    [extern printf_implementation]
    call printf_implementation

    mov rsp, rbp
    pop rbp
    ret