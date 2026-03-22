[bits 64]
section .text
global port_inb
port_inb:
    xor eax, eax
    mov edx, edi
    in al, dx
    ret

global port_outb
port_outb:

    mov eax, esi
    mov edx, edi
    out dx, al
    ret

global port_inw
port_inw:
    xor eax, eax
    mov edx, edi
    in ax, dx
    ret

global port_outw
port_outw:
    mov eax, esi
    mov edx, edi
    out dx, ax
    ret