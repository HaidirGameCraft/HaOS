; A20 Line - Accessing the Far Address
;
; When the A20 Line is enable, we can access the other memory address
; to check, we need to check Two memory address, and set the First and Second is Random Value
; for x86, [es:di], [ds:si] -> should be use
; - if there are same value, A20 Line is Disabled (we can't access other memory address)
; - if there are different, A20 Line is Enable and good to access memory address

A20Check:
    push bx
    push es
    push ds
    push di
    push si

    ; es -> 0x0000
    xor ax, ax
    mov es, ax
    mov di, 0x0510

    ; ds -> 0xFFFF
    xor ax, ax
    not ax
    mov ds, ax
    mov si, 0x0510

    ; save the old value
    mov ax, word [es:di]
    push ax
    mov ax, word [ds:si]
    push ax

    mov ax, 0x0050
    mov word [es:di], ax
    mov ax, 0x0040
    mov word [ds:si], ax

    mov ax, word [es:di]
    mov bx, word [ds:si]
    cmp ax, bx
    
    ; restore the old value
    pop ax
    mov word [ds:si], ax
    pop ax
    mov word [es:di], ax

    mov ax, 1
    jne .exit   ; the A20 Line is Enable
    mov ax, 0
.exit:
    pop si
    pop di
    pop ds
    pop es
    pop bx
    ret

; to enable A20 Line, we need to give the command into Keyboard Port 0x64
; - 1. Disable Keyboard Controller (0xAD)
; - 2. Read the Input from Controller (0xD0)
; - 3. Write the Input to Controller (0xD1), sending the data to (0x64)
; - 4. Enable Keyboard Controller (0xAE)
A20Enable:
    ; disable keyboard from controller
    call A20wait
    mov al, 0xAD
    mov dx, 0x64
    out dx, al

    call A20wait
    mov al, 0xD0
    mov dx, 0x64
    out dx, al

    call A20wait
    xor ax, ax
    mov dx, 0x60
    in al, dx

    push ax
    call A20wait
    xor ax, ax
    mov al, 0xD1
    mov dx, 0x64
    out dx, al

    pop ax
    or ax, 2
    mov dx, 0x60
    out dx, al

    ; enable keyboard controller
    mov al, 0xAE
    mov dx, 0x64
    out dx, al
    ret
A20wait:
    xor ax, ax
    mov dx, 0x64    ; the Status Port
    in al, dx
    and ax, 0b11
    test ax, ax
    jnz A20wait
    ret
