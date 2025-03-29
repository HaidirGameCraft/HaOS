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

%include "boot/boot_stage/x86/gdt.asm"
hello_message: db "Hello, World!", 0x0

[bits 32]
[extern vga_clearscreen]
[extern printf]
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

	; call vga_clearscreen
	
	[extern boot_stage_main]
	call boot_stage_main

    hlt
    jmp $

global Check_CPUID
Check_CPUID:
	mov eax, check_cpuid
	push eax
	call printf
	pop eax

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

	mov eax, supported_cpuid
	push eax
	call printf
	pop eax
	ret
.NoCPUID:
	mov eax, unsupported_cpuid
	push eax
	call printf
	pop eax

	mov eax, continueing_32bits
	push eax
	call printf
	pop eax
	ret

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

	mov eax, supported_longmode
	push eax
	call printf
	pop eax
	ret

.no_long_mode:
	mov eax, unsupported_longmode
	push eax
	call printf
	pop eax

	mov eax, continueing_32bits
	push eax
	call printf
	pop eax
	ret

global HALT 
HALT:
	cli
	hlt
	jmp $

global Enable_Paging
Enable_Paging:
	; Insert Page Dir
	mov cr3, eax

%ifdef __OSX86_64__
	; Enabling PAE-Paging for 64 Bits
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr
%endif

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
check_cpuid: db "Checking CPUID is supported for 64Bits", 0xa, 0x0
check_longmode: db "Checking LongMode is supported for 64Bits", 0xa, 0x0

unsupported_cpuid: db "Unsupported CPUID for 64Bits", 0xa, 0x0
supported_cpuid: db "supported CPUID for 64Bits", 0xa, 0x0

unsupported_longmode: db "Unsupported LongMode for 64Bits", 0xa, 0x0
supported_longmode: db "supported LongMode for 64Bits", 0xa, 0x0

continueing_32bits: db "Continuein 32Bits", 0xa, 0x0
hello_message_32bit: db "Hello, Welcome to World", 0xd, 0x0
word_hex: db "0123456789ABCDEF", 0xd, 0x0