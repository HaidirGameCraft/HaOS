section .text
%define ATA_PRIMARY_BUS 0x1F0
global ata_init_pio
ata_init_pio:
    ; Set the Drive Register
    mov dx, ATA_PRIMARY_BUS
    add dx, 6

    mov al, 0xE0
    mov ecx, dword [esp + 4]
    shr ecx, 24
    and cl, 0x0F
    add al, cl
    out dx, al

    ; Set register count sector
    mov dx, ATA_PRIMARY_BUS
    add dx, 2
    mov al, 1
    out dx, al

    ; Set LBA (low, mid, high)
    inc dx
    mov eax, dword [esp + 4]
    out dx, al

    inc dx
    shr eax, 8
    out dx, al

    inc dx
    shr eax, 8
    out dx, al
    ret

section .bss
ata_tmp_buffer: resb 0x200