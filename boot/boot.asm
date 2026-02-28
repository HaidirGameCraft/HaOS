[org 0x7C00]
[bits 16]

jmp short __boot_start
    nop
times 59 db 0

__boot_start:
    mov bp, 0x7C00
    mov sp, bp

    ; save the DRIVE NUMBER
    mov [__drive_number], dl

    ; Checking A20 Enable
    call A20Check
    test ax, ax
    jz .a20_disable_status

    mov si, a20_line_enable_msg
    call print

    jmp .a20_end_check
.a20_disable_status:
    mov si, a20_line_disable_msg
    call  print

    ; enable A2
    call A20Enable
.a20_end_check:



    ; Configure DAP
    mov ax, 1
    mov word [DAP + DiskAddressPacket.count], ax

    xor eax, eax
    mov ax, buffer
    mov dword [DAP + DiskAddressPacket.buffer], eax

    xor eax, eax
    mov ax, 1
    mov dword [DAP + DiskAddressPacket.start], eax 


    ; Read Extended Sector
    mov ah, 42h
    mov dl, byte [__drive_number]
    mov si, DAP
    int 13h

    ; Copy into BootStage Header
    mov si, buffer
    mov di, __bootstage_header
    mov cx, 22
.copy_loop:
    mov al, byte [si]
    mov byte [di], al
    inc si
    inc di
    dec cx
    test cx, cx
    jnz .copy_loop

    ; Copy the Bootstage into destination
    mov ecx, dword [__bootstage_header + BootStage_Header.bootEnd]
    mov eax, dword [__bootstage_header + BootStage_Header.bootStart]
    mov ebx, 1      ; starting LBA Address
    sub ecx, eax

    cmp ecx, 0
    jle .done_read_disk
.loop_read_disk:

    mov word [DAP + DiskAddressPacket.buffer], ax
    push eax
    shl eax, 16
    mov word [DAP + DiskAddressPacket.buffer + 2], ax
    mov dword [DAP + DiskAddressPacket.start], ebx

    ; Read Extended Sector
    xor eax, eax
    mov ah, 42h
    mov dl, byte [__drive_number]
    mov si, DAP
    int 13h

    pop eax
    inc ebx
    add eax, 512
    sub ecx, 512
    cmp ecx, 0
    jg .loop_read_disk
.done_read_disk:

    mov si, __bootstage_header
    call print

    mov eax, dword [__bootstage_header + BootStage_Header.bootEntry]
    jmp eax
    hlt
    jmp $

%include "a20.asm"

; si - buffer
print:
    xor ax, ax
.loop:
    mov al, byte [si]
    cmp al, 0
    je .done

    inc si
    mov ah, 0x0e
    int 10h
    jmp .loop
.done:
    ret

struc BootStage_Header
    BootStage_Header.bootIdentifier resb 10
    BootStage_Header.bootStart      resd 1
    BootStage_Header.bootEnd        resd 1
    BootStage_Header.bootEntry      resd 1
endstruc

struc DiskAddressPacket
    DiskAddressPacket.size resb 1
    DiskAddressPacket.unused resb 1
    DiskAddressPacket.count resw 1
    DiskAddressPacket.buffer resd 1
    DiskAddressPacket.start resq  1
endstruc

__drive_number: db 0
__bootstage_header:
istruc BootStage_Header
    at BootStage_Header.bootIdentifier,     db "         ", 0
    at BootStage_Header.bootStart,          dd 0
    at BootStage_Header.bootEnd,            dd 0
    at BootStage_Header.bootEntry,          dd 0
iend

DAP:
istruc DiskAddressPacket
    at DiskAddressPacket.size, db 0x10
    at DiskAddressPacket.unused, db 0x00
    at DiskAddressPacket.count, dw 0x01
    at DiskAddressPacket.buffer, dd 0x00000000
    at DiskAddressPacket.start, dq 0x0000000000000000
iend

a20_line_enable_msg: db "A20 Line Enable", 0xa, 0xd, 0
a20_line_disable_msg: db "A20 Line Disable", 0xa, 0xd, 0

times 510-($-$$) db 0
dw 0xAA55
buffer: