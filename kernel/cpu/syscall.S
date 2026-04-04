.section .text
.globl int_syscall
.extern syscall_handle
int_syscall:
    cli
    // Faking Error code
    pushq $0
    pushq $0x80

    pushq %r15
    pushq %r14
    pushq %r13
    pushq %r12
    pushq %r11
    pushq %r10
    pushq %r9
    pushq %r8

    pushq %rsi 
    pushq %rdi
    pushq %rbp
    pushq %rsp
    pushq %rbx
    pushq %rdx
    pushq %rcx
    pushq %rax

    mov %cr0, %rax
    push %rax

    mov %cr2, %rax
    push %rax

    mov %cr3, %rax
    push %rax

    mov %cr4, %rax
    push %rax

    movq 40(%rsp), %rax
    call syscall_handle

    addq $176, %rsp
    sti
    iretq


.globl read
read:
    mov $0, %rax
    int $0x80
    ret

.globl open
open:
    mov $2, %rax
    int $0x80
    ret

.globl close
close:
    mov $3, %rax
    int $0x80
    ret

.globl put_pixel
put_pixel:
    mov $4, %rax
    int $0x80
    ret

.globl putc
putc:
    mov $5, %rax
    int $0x80
    ret

.globl exit
exit:
    mov $6, %rax
    int $0x80
    ret

