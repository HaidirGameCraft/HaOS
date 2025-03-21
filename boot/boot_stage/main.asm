

[bits 16]
section .data
global vesa_info_block
global vesa_mode_info
vesa_info_block: resb 512
vesa_mode_info: resb 91 + 206

section .text
__start:

	mov sp, 0x1000
	mov bp, sp

	mov si, hello_message
	call print_

	mov ax, 0x4f02
	mov bx, 0x4115
	int 0x10

	mov ax, 0x4f00
	mov di, vesa_info_block
	int 0x10

	mov ax, 0x4f01
	mov cx, 0x0115
	mov di, vesa_mode_info
	int 0x10

	cli 
	lgdt [gdt_desc]

	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp 0x08:protected_mode

print_:
	push si
.put:
	mov ah, 0x0e
	mov al, byte [si]
	inc si
	int 0x10

	test al, al
	jnz .put

	pop si
	ret

%include "boot/boot_stage/gdt.asm"
hello_message: db "Hello, World!", 0x0

[bits 32]
[extern vga_clearscreen]

protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

	mov ebp, 0x90000
	mov esp, ebp

	; call vga_clearscreen
	
	[extern boot_stage_main]
	call boot_stage_main

    hlt
    jmp $

section .data
hello_message_32bit: db "Hello, Welcome to World", 0xd, 0x0
word_hex: db "0123456789ABCDEF", 0xd, 0x0