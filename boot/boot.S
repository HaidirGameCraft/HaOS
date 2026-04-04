[org 0x7c00]
[bits 16]

__boot_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov sp, 0x7c00
    mov bp, sp

    mov ah, 0x0e
    mov al, 'H'
    int 0x10

    mov al, 'e'
    int 0x10

    mov al, 'l'
    int 0x10

    int 0x10

    mov al, 'o'
    int 0x10

halt:
    hlt
    jmp halt

print:
    xor ax, ax

    mov ah, 0x0e
.loop:
    mov al, byte [si]
    inc si
    cmp al, 0
    je .done

    int 0x10
    jmp .loop
.done:
    ret

message: db "Hello, World", 0

times 510-($-$$) db 0
dw 0xAA55
