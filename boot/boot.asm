[org 0x7c00]
[bits 16]

jmp short _start
	nop

fat_header:
.oem: db "MSWIN4.1"
.bytes_per_sector: dw 0x200
.sectors_per_cluster: db 0x01
.reserved_sectors: dw 0x20
.fat_number: db 0x02
.root_dir_entries: dw 0x00
.total_sectors: dw 0x00
.media_type: db 0xF8
.reserved_fat: dw 0x00
.sectors_per_track: dw 63
.heads: dw 16
.hidden_sectors: dd 0x00
.large_sector: dd 0x020260
.sector_per_FAT: dd 0x3f5
.flags: dw 0x00
.version_number: dw 0x00
.cluster: dd 0x02
.FSInfo_struct: dw 0x01
.backup_boot_sector: dw 0x06
.reserved: dq 0
			dd 0
.drive_number: db 0x80
				db 0
.signature: db 0x29
.volume_id: dd 0xEDC1B961
.volume_label: db "Local Disk "
.identifier: db "FAT32  "


_start:
	mov sp, 0x7c00
	mov bp, sp

	mov ax, 2
	mov bx, 0x7e00
	mov cx, 4
	mov dl, [fat_header.drive_number]
	call read_disk

	jmp 0x07e0:0x0000

	hlt
	jmp $

%include "boot/read_disk.asm"
%include "boot/print.asm"

times 510-($-$$) db 0
dw 0xAA55
