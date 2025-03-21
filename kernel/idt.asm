section .text
global idt_install
idt_install:
    mov eax, dword [esp + 4]
    lidt [eax]
    sti
    ret