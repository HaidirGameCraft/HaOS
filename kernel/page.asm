section .text
global page_enable
page_enable:
    mov rax, rdi
    mov cr3, rax

    mov rax, cr0
    or eax, 0x80000001
    mov cr0, rax

    ret