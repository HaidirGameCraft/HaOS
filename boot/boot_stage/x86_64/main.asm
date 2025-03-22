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

%include "boot/boot_stage/x86/gdt.asm"
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

	call main

    hlt
    jmp $

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

	mov ax, 0x0F00 | 'S'
	mov word [0xb8000], ax
	mov ax, 0x0F00 | 'C'
	mov word [0xb8002], ax
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

	mov ax, 0x0F00 | 'S'
	mov word [0xb8000], ax
	mov ax, 0x0F00 | 'L'
	mov word [0xb8002], ax
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

%ifdef __OSX86_64__
global Go_To_LongMode
Go_To_LongMode:
	cli
	lgdt [gdt64_desc]
	jmp 0x08:long_mode

[bits 64]
long_mode:
	mov ax, 0x10
	mov ds, ax
	mov es, ax 
	mov ss, ax 
	mov fs, ax 
	mov gs, ax 

	ret
%endif

section .data
word_hex: db "0123456789ABCDEF", 0xd, 0x0