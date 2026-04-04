.section .text
.globl go_jump
go_jump:
    mov $0x15000, %rax
    jmp *%rax