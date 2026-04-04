[bits 32]
section .text
; Initialize Serial Port for Debugging
global serial_init
serial_init:
    xor edx, edx
    ; Disable Interrupt Register
    mov dx, bx
    inc dx
    mov al, 0
    out dx, al

    ; Set the DLAB Bit
    mov dx, bx
    add dx, 3
    mov al, (1 << 7) ; DLAB Bit
    out dx, al

    ; Set the least significant Baud Rate
    mov dx, bx
    mov al, 3
    out dx, al

    ; Set the most significant Baud rate
    mov dx, bx
    inc dx
    xor ax, ax
    out dx, al

    ; Clear the most DLAB Bit on Line Control
    ; Set Data bits
    mov dx, bx
    add dx, 3
    mov al, 0b11
    out dx, al

    mov dx, bx
    add dx, 2
    mov al, 0xC7    ; set up FIFO
    out dx, al

    ; Enable IRQ
    mov dx, bx
    add dx, 4
    mov al, 0x0B
    out dx, al

    ; Set up loopback mode
    mov dx, bx
    add dx, 4
    mov al, 0x1E
    out dx, al

    ; Send The Byte to check it is work
    mov dx, bx
    mov al, 0xCE
    out dx, al

    xor ax, ax
    in al, dx
    cmp al, 0xCE
    je .no_fault
    mov eax, 1
    ret

.no_fault:
    mov dx, bx
    add dx, 4
    mov al, 0xF

    xor eax, eax
    ret

global serial_read
serial_read:
    mov bx, word [esp + 4]
    ; Checking the Status either it data ready and doesnt have anu parity error
    mov dx, bx
    add dx, 5
.loop:
    xor ax, ax
    in al, dx
    and al, 1
    test al, al ; Data Ready
    jz .loop

    sub dx, 5
    xor eax, eax
    in al, dx
    ret

global serial_write
serial_write:
    mov bx, word [esp + 4]

    mov dx, bx
    add dx, 5
.loop:
    xor ax,  ax
    in al, dx
    and al, 0x20
    test al, al ; Buffer is ready to write
    jz .loop

    mov al, byte [esp + 8]
    sub dx, 5
    out dx, al
    ret

global asm_serial_print
asm_serial_print:
    ; esi as buffer/text
    push eax
    xor eax, eax
.loop:
    mov al, byte [esi]
    cmp al, 0
    je .done
    push eax
    push 0x3F8
    call serial_write
    add esp, 8

    inc esi
    jmp .loop
.done:
    pop eax
    ret

[bits 64]
section .text
global x64_serial_read
x64_serial_read:
    xor rax, rax
    mov bx, di
    ; Checking the Status either it data ready and doesnt have anu parity error
    mov dx, bx
    add dx, 5
.loop:
    xor ax, ax
    in al, dx
    and al, 1
    test al, al ; Data Ready
    jz .loop

    sub dx, 5
    xor eax, eax
    in al, dx
    ret

global x64_serial_write
x64_serial_write:
    xor rax, rax
    mov bx, di

    mov dx, bx
    add dx, 5
.loop:
    xor ax,  ax
    in al, dx
    and al, 0x20
    test al, al ; Buffer is ready to write
    jz .loop

    mov ax, si
    sub dx, 5
    out dx, al
    ret
