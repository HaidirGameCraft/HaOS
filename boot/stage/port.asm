section .text
global port_inb
port_inb:
    xor eax, eax
    mov dx, word [esp + 4]
    in al, dx
    ret

global port_outb
port_outb:
    mov al, byte [esp + 8]
    mov dx, word [esp + 4]
    out dx, al
    ret

global port_inw
port_inw:
    xor eax, eax
    mov dx, word [esp + 4]
    in ax, dx
    ret

global port_outw
port_outw:
    mov ax, word [esp + 8]
    mov dx, word [esp + 4]
    out dx, ax
    ret