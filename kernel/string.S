.section .text
.globl memzero
memzero:
    push %rbp
    mov %rsp, %rbp
    subq $24, %rsp

    mov %rdi, %rax
    movq %rax, -24(%rbp)

    mov %rsi, %rax
    movq %rax, -16(%rbp)

    movq $0, -8(%rbp)
    jmp .cnd
.loop:
    
    movq -24(%rbp), %rax
    mov %rax, %rsi
    movb $0, (%rsi)
    incq %rax
    movq %rax, -24(%rbp)


    // Increase i
    movq -8(%rbp), %rax
    addq $1, %rax
    movq %rax, -8(%rbp)
.cnd:   // i < size
    movq -8(%rbp), %rax
    cmpq -16(%rbp), %rax
    jb .loop

    mov %rbp, %rsp
    pop %rbp
    ret


