.section .text
.globl printf
.extern printf_implementation
printf:
    push %rbp
    mov %rsp, %rbp

    push %r9
    push %r8
    push %rcx
    push %rdx
    push %rsi
    push %rdi

    mov %rsp, %rdi
    call printf_implementation

    mov %rbp, %rsp
    pop %rbp
    ret