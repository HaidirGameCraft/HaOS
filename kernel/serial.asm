section .text
global serial_connect
serial_connect:
    push rbp
    mov rbp, rsp

    mov ebx, edi        ; rdi - Port
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
    mov rax, 1

    mov rsp, rbp
    pop rbp
    ret

.no_fault:
    mov dx, bx
    add dx, 4
    mov al, 0xF

    xor rax, rax
    mov rsp, rbp
    pop rbp
    ret

global serial_disconnect
serial_disconnect:
    mov ebx, edi            ; rdi - Port
    ; Disable Interrupt
    mov dx, bx
    inc dx      ; Port + 1
    mov al, 0
    out dx, al

    ; Disble DLAB
    mov dx, bx
    add dx, 3
    xor ax, ax
    out dx, al

    ; Disable FIFO
    mov dx, bx
    inc dx
    mov al, 0x06
    out dx, al

    ; Clearing all FIFO
    mov dx, bx
    add dx, 4
    mov al, 0x00
    out dx, al
    ret



global serial_read
serial_read:
    mov ebx, edi
    mov dx, bx
.wait:
    xor rax, rax
    add dx, 5
    in al, dx
    and al, 1
    test al, al
    jz .wait

    sub dx, 5
    in al, dx
    ret

global serial_write
serial_write:
    mov ebx, edi
    mov dx, bx
.wait:
    xor eax, eax
    add dx, 5
    in al, dx
    and al, 0x20
    test al, al
    jz .wait

    mov rax, rsi
    sub dx, 5
    out dx, al
    xor rax, rax
    ret


global serial_printf
serial_printf:
    push rbp
    mov rbp, rsp 

    push r9
    push r8
    push rcx
    push rdx
    push rsi
    push rdi
    
    mov rdi, rsp
    [extern serial_printf_implementation]
    call serial_printf_implementation


    mov rsp, rbp
    pop rbp
    ret