[org 0x7e00]
[bits 16]
jmp __start

%include "boot/read_disk.asm"
%include "boot/print.asm"

fat_sector_start: dw 0
fat_data_sector: dw 0
cluster: dd 2
backup: dd 0

__start:
    mov ax, word [FAT_HEADER + FAT_RESERVED_SECTORS]
    mov word [fat_sector_start], ax

    
    mov ax, word [FAT_HEADER + FAT_SECTOR_PER_FAT]
    xor cx, cx
    mov cl, byte [FAT_HEADER + FAT_FAT_NUMBERS]
    mul cx
    add ax, word [fat_sector_start]
    mov word [fat_data_sector], ax

.search_file:
    mov eax, dword [cluster]
    sub ax, 2
    xor cx, cx
    mov cl, byte [FAT_HEADER + FAT_SECTOR_PER_CLUSTER]
    mul cx
    add ax, word [fat_data_sector]

    ; mov ax, 0x80A
    mov cx, 1
    mov bx, buffer
    mov dl, byte [FAT_HEADER + FAT_DRIVE_NUMBER]
    call read_disk

.find_file:
    mov di, boot_stage_name
    mov si, buffer + 32
    
    mov cx, 11
.match_name:
    mov al, byte [si]
    cmp al, byte [di]
    jne .failed

    dec cx
    inc si
    inc di
    test cx, cx
    jne .match_name

    mov ax, word [ buffer + 32 + 26 ]
    mov word [cluster], ax

    mov ebx, 0x1000
    ;call convert_linear_to_segment

    jmp .read_file

.failed:
    mov si, file_stage_not_found
    call print_

    hlt
    jmp $
.read_file:
    mov edi, cluster
    call print_hex

    mov eax, dword [cluster]
    sub ax, 2
    xor cx, cx
    mov cl, byte [FAT_HEADER + FAT_SECTOR_PER_CLUSTER]
    mul cx
    add ax, word [fat_data_sector]

    mov cx, 1
    mov dl, byte [FAT_HEADER + FAT_DRIVE_NUMBER]
    call read_disk

    add ebx, 0x200
    push ebx
    xor ebx, ebx
    
    mov eax, dword [cluster]
    mov cx, 0x80
    xor dx, dx
    div cx


    push dx

    add ax, word [fat_sector_start]
    mov bx, 0
    mov es, bx
    mov bx, buffer
    mov cx, 1
    mov dl, byte [FAT_HEADER + FAT_DRIVE_NUMBER]
    call read_disk

    pop dx
    mov ax, dx
    mov cx, 4
    mul cx

    mov si, ax
    add si, buffer
    mov eax, dword [esi]
    mov dword [cluster], eax

    pop ebx
    mov eax, dword [cluster]
    shr eax, 24
    and al, 0x0F
    cmp al, 0x0F 
    jge .finish

    jmp .read_file

.finish:
    mov si, load_file_message
    call print_

    mov ax, 0
    mov ds, ax
    mov es, ax 
    mov ss, ax 
    mov fs, ax 
    mov gs, ax 
    jmp 0:0x1000

convert_linear_to_segment:
    push ebx
    shr ebx, 16
    mov es, bx
    pop ebx
    ret

file_stage_not_found: db "Error Stage Not Found", 0xa, 0xd, 0x0
load_file_message: db "Load Stage...", 0xa, 0xd, 0x0
test_message: db "Test!", 0xa, 0xd, 0x00
boot_stage_name: db "BOOTS   BIN"
buffer: resb 512
