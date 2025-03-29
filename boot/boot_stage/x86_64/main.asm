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

%include "boot/boot_stage/x86_64/gdt.asm"
hello_message: db "Hello, World!", 0x0

[bits 32]
[extern main]

section .text
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

	mov ebp, 0x90000
	mov esp, ebp

	call Check_CPUID
	call Check_LongMode

	mov eax, pdp_page
	or eax, 0b11
	mov dword [pml4t_page], eax

	mov eax, pd_page
	or eax, 0b11
	mov dword [pdp_page], eax

	mov eax, pd_page
	or eax, 0b11
	mov dword [pd_page], eax

	mov eax, first_pt_page
	or eax, 0b11
	mov dword [pd_page], eax

	mov eax, page_pt
	or eax, 0b11
	mov dword [pd_page + 16], eax

	mov ecx, 0
.filled:
	mov eax, 0x1000
	mul ecx
	or eax, 0b11 ; READ_WRITE_PAGE | PRESENT_PAGE
	mov dword [first_pt_page + ecx * 8], eax

	add eax, 0x400000
	mov dword [page_pt + ecx * 8], eax

	inc ecx
	cmp ecx, 512
	jne .filled

	mov eax, pml4t_page
	mov cr3, eax

	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax

    lgdt [gdt64_desc]
	jmp 0x08:long_mode

global Check_CPUID
Check_CPUID:
	pushfd
	pop eax
	
	mov ecx, eax
	xor eax, 1 << 21

	push eax
	popfd

	pushfd
	pop eax
	push ecx
	popfd

	xor eax, ecx
	jz .NoCPUID

	ret
.NoCPUID:
	mov ax, 0x0F00 | 'U'
	mov word [0xb8000], ax
	mov ax, 0x0F00 | 'S'
	mov word [0xb8002], ax
	mov ax, 0x0F00 | 'C'
	mov word [0xb8004], ax

	hlt
	jmp $

global Check_LongMode
Check_LongMode:
	mov eax, 0x80000000
	cpuid 
	cmp eax, 0x80000001
	jb .no_long_mode

	mov eax, 0x80000001
	cpuid 
	test edx, 1 << 29
	jz .no_long_mode

	ret

.no_long_mode:
	mov ax, 0x0F00 | 'U'
	mov word [0xb8000], ax
	mov ax, 0x0F00 | 'S'
	mov word [0xb8002], ax
	mov ax, 0x0F00 | 'L'
	mov word [0xb8004], ax

	hlt 
	jmp $

global Halt 
Halt:
	cli
	hlt
	jmp $

global Enable_Paging
Enable_Paging:                              ; Set control register 0 to the A-register.
	; Insert Page Dir
	mov eax, [esp + 0x4]
	mov cr3, eax
	
	; Enabling PAE-Paging for 64 Bits
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	; Enable Paging
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret


section .bss
align 0x1000
pml4t_page: equ 0x400000
; pdp_page: equ 0x401000
; pd_page: equ 0x402000
pdp_page: equ 0x401000
pd_page: equ 0x402000
first_pt_page: equ 0x403000
page_pt: equ 0x404000


section .rodata
gdt64:
.null:
    dq 0x00000000
.code:
    dw 0xFFFF
    dw 0x0000
	db 0
    db 0x9A
    db 0xAF
    db 0
.data:
    dw 0xFFFF
    dw 0x0000
	db 0
    db 0x92
    db 0xAF
    db 0
gdt64_desc:
    dw gdt64_desc - gdt64 - 1
    dd gdt64

[bits 64]
[extern boot_stage_main]
section .text
long_mode:
	mov ax, 0x10
	mov ds, ax
	mov es, ax 
	mov ss, ax 
	mov fs, ax 
	mov gs, ax 

	call boot_stage_main

	hlt
	jmp $



section .data
word_hex: db "0123456789ABCDEF", 0xd, 0x0
