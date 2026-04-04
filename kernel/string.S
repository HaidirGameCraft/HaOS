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
    jmp 2f

1:
    movq -24(%rbp), %rsi
    movb $0, (%rsi)
    inc  %rsi
    movq %rsi, -24(%rbp)


    // Increase i
    movq -8(%rbp), %rax
    inc  %rax
    movq %rax, -8(%rbp)
2:   // i < size
    movq -8(%rbp), %rax
    cmpq -16(%rbp), %rax
    jb 1b

    mov %rbp, %rsp
    pop %rbp
    ret

.globl memcopy
memcopy:
    push %rbp
    mov %rsp, %rbp
    sub $0x10, %rsp

    // initialize dest, src, size and i
    mov %rdx, -0x10(%rbp)
    mov $0, -0x08(%rbp)
    xor %rax, %rax

    jmp 2f
1:
    movb (%rsi), %al
    movb %al, (%rdi)

    inc %rdi
    inc %rsi
    
    mov -0x08(%rbp), %rax
    inc %rax
    mov %rax, -0x08(%rbp)
2:
    xor %rax, %rax
    movl -0x08(%rbp), %eax
    cmpq %rdx, %rax
    jb 1b

    mov %rbp, %rsp
    pop %rbp
    ret


