section .text
global gdt_install
gdt_install:
    mov eax, dword [esp + 4]
    lgdt [eax]

    jmp 0x08:.flush
.flush:
    ret