.section .text
.globl page_enable
page_enable:
    mov %rdi, %rax
    mov %rax, %cr3

    mov %cr0, %rax
    or  $0x80000001, %eax
    mov %rax, %cr0

    ret
