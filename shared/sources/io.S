.section .text
.global printf

.extern printf_impl
printf:
    push %rbp
    movq %rsp, %rbp

    push %r9
    push %r8
    push %rcx
    push %rdx
    push %rsi
    push %rdi

    movq %rsp, %rdi
    call printf_impl

    movq %rbp, %rsp
    pop %rbp
    ret
