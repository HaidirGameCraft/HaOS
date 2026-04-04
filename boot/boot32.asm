[bits 16]
section .header
    dd go_protected_mode

section .text
go_protected_mode:
    mov ax, 0x4F00
    mov di, 0x2000 ; Vesa Info Address
    int 0x10

    ; Get The Vesa Mode Information
    mov ax, 0x4F01
    mov cx, 0x0115
    mov di, 0x3000 ; Vesa Mode Address
    int 0x10

    ; Set the Vesa Video Mode
    mov ax, 0x4F02
    mov bx, cx
    or bx, 0x4000
  ;  int 0x10

    cli
    lgdt [gdt32.desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:__protected_mode
    
section .data
gdt32:
.null:  dq 0x00000000
.code:  dd 0x0000ffff
        dd 0x00cf9a00
.data:  dd 0x0000ffff
        dd 0x00cf9200
.desc:  dw $ - gdt32 - 1
        dd gdt32


[bits 32]
%define FIXED_BOOTSTAGE_LBA 2
%define BOOTSTAGE_BUFFER    0x8000
section .text
__protected_mode:

    mov ax, 0x10
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov gs, ax 
    mov fs, ax

    ; now, read disk
    mov edi, FIXED_BOOTSTAGE_LBA
    mov esi, BOOTSTAGE_BUFFER
    call __ata_read_disk

    ; copy header
.copy_header:
    mov edi, bootstage_header
    mov esi, BOOTSTAGE_BUFFER
    mov ecx, 22

.loop_copy:
    mov al, byte [esi]
    mov byte [edi], al
    inc esi
    inc edi
    dec ecx
    cmp ecx, 0
    jg .loop_copy

    mov esi, bootstage_header
    call print

    mov esi, dword [bootstage_header + 10] ; boot address start
    mov edi, FIXED_BOOTSTAGE_LBA
.read_disk:
    push edi
    push esi
    push eax
    call __ata_read_disk
    pop eax
    pop esi
    pop edi
    
    inc edi
    add esi, 512
    cmp esi, dword [bootstage_header + 14]  ; boot address end
    jl .read_disk

    mov eax, dword [bootstage_header + 18]   ; boot entry address
    jmp eax ; jump to bootstage
    hlt
    jmp $

print:
    push esi
    mov ebx, 0xb8000
    mov ah, 0x0f
.loop:
    mov al, byte [esi]
    inc esi
    cmp al, 0
    je .done

    mov word [ebx], ax
    add ebx, 2

    jmp .loop
.done:
    pop esi
    ret
__ata_read_disk:
    ; edi - lba
    ; esi - buffer
    ; must be 512

    mov dx, 0x1F6 
    mov eax, edi
    shr eax, 24
    and al, 0x0F
    or al, 0xE0    ; master
    out dx, al      ; select the disk + set the lba 24

    mov dx, 0x1F2
    mov al, 1
    out dx, al      ; set read sector count

    inc dx
    mov eax, edi
    out dx, al      ; set lba:0

    inc dx
    shr eax, 8
    out dx, al      ; set lba:8

    inc dx
    shr eax, 8
    out dx, al      ; set lba:16

    mov dx, 0x1F7
    mov al, 0x20
    out dx, al      ; send READ command
.wait_busy_disk:
    in al, dx
    test al, 0x80   ; read BUSY status from disk
    jnz .wait_busy_disk

    mov ecx, 0
    mov dx, 0x1F0
.read_disk
    in ax, dx       ; read data with size word
    mov word [esi], ax
    add esi, 2
    add ecx, 2
    cmp ecx, 512
    jl .read_disk

    ret

section .data
test_msg: db "HELLO", 0

section .bss
bootstage_header:
    resb 10
    resd 3
