section .text
global port_inb
port_inb:
    mov rdx, rdi
    xor rax, rax
    in al, dx
    ret

global port_inw
port_inw:
    mov rdx, rdi
    xor rax, rax
    in ax, dx
    ret

global port_ind
port_ind:
    mov rdx, rdi 
    xor rax, rax
    in eax, dx
    ret

global port_outb
port_outb:
    mov rax, rsi
    mov rdx, rdi
    out dx, al
    ret

global port_outw
port_outw:
    mov rax, rsi
    mov rdx, rdi
    out dx, ax
    ret

global port_outd
port_outd:
    mov rax, rsi
    mov rdx, rdi
    out dx, eax
    ret