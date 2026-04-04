.section .text
.globl go_to_kernel
go_to_kernel:
    mov %rdi, %rax
    mov %rax, %cr3

    mov %cr4, %rax
    or $0x20, %rax
    mov %rax, %cr4

    mov $0xC0000080, %ecx
    rdmsr
    or 0x100, %rax
    wrmsr

    mov %cr0, %rax
    or $0x80000001, %eax
    mov %rax, %cr0

    mov %rsi, %rax
    jmp *%rax