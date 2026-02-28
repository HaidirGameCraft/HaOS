[bits 16]

section .header
[extern __BOOTSTAGE_START]
[extern __BOOTSTAGE_END]
identifier: db "BOOTSTAGE", 0
bootstage_start: dd __BOOTSTAGE_START
bootstage_end: dd __BOOTSTAGE_END
entry_bootstage_start: dd __start

section .text
__start:

    [extern vesa_init]
    call vesa_init

    ; load gdt
    cli
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode

section .data
gdt:
.null: dq 0
.code:  dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x9A
        db 0xFC
        db 0x00
.data:  dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x92
        db 0xFC
        db 0x00
gdt_desc:   dw $ - gdt - 1
            dd gdt

[bits 32]
%define COM1 0x3F8
section .text
protected_mode:

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax

    mov esp, stack_bottom
    mov ebp, esp

    call serial_init

    [extern main]
    call main
.halt:
    hlt
    jmp .halt

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

section .bss
stack_top: resb 0x2000
stack_bottom: