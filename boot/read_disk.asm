%define FAT_HEADER 							0x7C00
%define FAT_OEM 							0x03
%define FAT_BYTES_PER_SECTOR 				0x0B
%define FAT_SECTOR_PER_CLUSTER 				0x0D
%define FAT_RESERVED_SECTORS				0x0E
%define FAT_FAT_NUMBERS 0x10
%define FAT_ROOT_ENTRIES 0x11
%define FAT_TOTAL_SECTORS 0X13
%define FAT_MEDIA_TYPE 0x15
%define FAT_SECTOR_PER_TRACK 				0x18
%define FAT_HEADS 							0x1A
%define FAT_HIDDEN_SECTORS 0x1C
%define FAT_LARGE_SECTOR	0x20

%define FAT_SECTOR_PER_FAT 0x24
%define FAT_FLAGS 0x28
%define FAT_VERSION_NUMBER 0x2A
%define FAT_CLUSTER_NUMBER 0x2C
%define FAT_FSINFO 0x30
%define FAT_BACKUP_BOOT 0x32

%define FAT_DRIVE_NUMBER 					0x40
%define FAT_VOLUME_STRING 0x47


read_disk:
	push ax
	push bx
	push cx
	push dx

.lba_to_chs:
	pusha
	xor dx, dx
	div word [FAT_HEADER + FAT_SECTOR_PER_TRACK]
	inc dl
	mov byte [.sector], dl

	xor dx, dx
	div word [FAT_HEADER + FAT_HEADS]
	mov byte [.head], dl

	mov byte [.cylinder], al

	mov di, 3
.read:
	mov ah, 0x02
	mov al, 1
	mov ch, byte [.cylinder]
	mov cl, byte [.sector]
	mov dh, byte [.head]
	mov dl, byte [FAT_HEADER + FAT_DRIVE_NUMBER]
	int 0x13
	jnc .done

	mov ah, 0x00
	int 0x13
	jc .reset_disk_error

	dec di
	test di, di
	jnz .read

	jmp .disk_error
.reset_disk_error:
	popa
	pop dx
	pop cx
	pop bx
	pop ax

	mov si, reset_disk_error_message
	call print_

	hlt
	jmp $
.disk_error:
	popa
	pop dx
	pop cx
	pop bx
	pop ax

	mov si, disk_error_message
	call print_
	
	hlt
	jmp $
.done:
	popa

	inc ax
	dec cx
	add bx, 512
	test cx, cx
	jnz .lba_to_chs

	pop dx
	pop cx
	pop bx
	pop ax
	; mov di, .sector
	; call print_hex
	
	; mov si, read_disk_done_message
	; call print_

	ret
	
.sector: db 0
.head: db 0
.cylinder: db 0

reset_disk_error_message: db "Reset Disk Error", 0xa, 0xd, 0x0
disk_error_message: db "Disk Error", 0xa, 0xd, 0x0
read_disk_done_message: db "Read Disk [Done]", 0xa, 0xd, 0x0
