section .text
global port_inb
port_inb:
    mov dx, word [esp + 4]
    xor eax, eax
    in al, dx
    ret

global port_inw
port_inw:
    mov dx, word [esp + 4]
    xor eax, eax
    in ax, dx
    ret

global port_ind
port_ind:
    mov dx, word [esp + 4]
    xor eax, eax
    in eax, dx
    ret

global port_outb
port_outb:
    mov al, byte [esp + 8]
    mov dx, word [esp + 4]
    out dx, al
    ret

global port_outw
port_outw:
    mov ax, word [esp + 8]
    mov dx, word [esp + 4]
    out dx, ax
    ret

global port_outd
port_outd:
    mov eax, dword [esp + 8]
    mov dx, word [esp + 4]
    out dx, eax
    ret