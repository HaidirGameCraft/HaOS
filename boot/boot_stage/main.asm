[bits 16]
__start:

	mov si, hello_message
	call print_

	jmp _install_GDT

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

hello_message: db "Hello, World!", 0x0

%include "boot/boot_stage/gdt.asm"

_install_GDT:
    cli 
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode

[bits 32]
protected_mode:

	mov ax, 0x0F00 | 'D'
	mov word [0xb8000], ax

	hlt
	jmp $

