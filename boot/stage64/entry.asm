[bits 16]

section .header
[extern __BOOTSTAGE_START]
[extern __BOOTSTAGE_END]
identifier: db "BOOTSTAGE", 0
bootstage_start: dd __BOOTSTAGE_START
bootstage_end: dd __BOOTSTAGE_END
entry_bootstage_start: dd __start

[extern serial_init]
[extern asm_serial_print]

section .text
__start:

    [extern vesa_init]
    call vesa_init

    ; load gdt
    cli
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode

section .data
gdt:
.null: dq 0
.code:  dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x9A
        db 0xFC
        db 0x00
.data:  dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x92
        db 0xFC
        db 0x00
gdt_desc:   dw $ - gdt - 1
            dd gdt

[bits 32]
%define COM1 0x3F8
section .text
protected_mode:

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov ss, ax
    mov gs, ax

    mov esp, stack_bottom
    mov ebp, esp
    call serial_init

    call check_cpuid_support
    cmp eax, 0
    jne .halt

    call check_longmode_support
    cmp eax, 0
    jne .halt

    jmp setup_to_longmode


    ; [extern main]
    ; call main
.halt:
    hlt
    jmp .halt

; Checking the Long Mode Support and CPUID
global check_cpuid_support
check_cpuid_support:
    pushfd
    pop eax

    mov ecx, eax ; save the eflags into different register
    or eax, ( 1 << 21 ) ; set the ID bits into eflags

    push eax
    popfd

    ; get and check whether the ID bits is remains unchanges or not
    pushfd
    pop eax

    xor eax, ecx
    push ecx
    popfd

    jnz .cpuid_support
    mov eax, 1

    mov esi, message_cpuid_unsupport
    call asm_serial_print

    ret
.cpuid_support:
    mov eax, 0

    mov esi, message_cpuid_support
    call asm_serial_print
    ret

global check_longmode_support
check_longmode_support:
    ; Get the information of CPU
    mov eax, 0x80000000
    cpuid

    cmp eax, 0x80000001
    jb .no_long_mode    ; Checking whether the is below, no long mode
    
    mov eax, 0x80000001
    cpuid

    and edx, (1 << 29)
    jz .no_long_mode

    jmp .long_mode_support
.no_long_mode:
    mov esi, message_longmode_unsupport
    call asm_serial_print

    mov eax, 1
    ret
.long_mode_support:
    mov esi, message_longmode_support
    call asm_serial_print

    mov eax, 0
    ret

section .data
message_cpuid_support: db "CPUID is Support", 0xA, 0
message_cpuid_unsupport: db "CPUID is Unsupport", 0xA, 0

message_longmode_support: db "Long Mode is Support", 0xA, 0
message_longmode_unsupport: db "Long Mode is Unsupport", 0x0A, 0

section .bss
stack_top: resb 0x2000
stack_bottom:

; Switching the Protected Mode into Long Mode
%define PAGE_BITMAP_ADDRESS     0x1000
%define PMLT4_ADDRESS           0x2000
%define PDPT_ADDRESS            0x3000
%define PDT_ADDRESS             0x4000
%define FIRST_PT_ADDRESS        0x5000
%define FRAME_PT_ADDRESS        0x6000

%define PAGE_ATTR_PRESENT       (1 << 0)
%define PAGE_ATTR_READWRITE     (1 << 1)


%define EFER_MSR                0xC0000080
%define EFER_LONGMODE_ENABLE    (1 << 8)
section .text
setup_to_longmode:
    ; Set up paging for 64 bits
    mov eax, PDPT_ADDRESS
    or eax, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE
    mov dword [PMLT4_ADDRESS], eax

    mov eax, PDT_ADDRESS
    or eax, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE
    mov dword [PDPT_ADDRESS], eax

    mov eax, FIRST_PT_ADDRESS
    or eax, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE
    mov dword [PDT_ADDRESS], eax

    mov eax, FRAME_PT_ADDRESS
    or eax, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE
    mov dword [PDT_ADDRESS + 8 * (511)], eax

.fill_first_pt_table:
    xor ecx, ecx
.loop:
    mov eax, ecx
    shl eax, 12
    or eax, PAGE_ATTR_PRESENT | PAGE_ATTR_READWRITE
    mov dword [FIRST_PT_ADDRESS + ecx * 8], eax

    inc ecx
    cmp ecx, 512
    jl .loop

    ; set pml4t and enable paging
    mov eax, PMLT4_ADDRESS
    mov cr3, eax

    mov eax, cr4
    or eax, (1 << 5)
    mov cr4, eax

    mov ecx, EFER_MSR
    rdmsr
    or eax, EFER_LONGMODE_ENABLE
    wrmsr
    
    mov eax, cr0
    or eax, 0x80000001  ; Enable Paging bits
    mov cr0, eax


    ; Setup GDT for 64bits
    cli
    lgdt [gdt64_desc]

    jmp 0x8:long_mode

.halt:
    hlt
    jmp .halt

section .data
gdt64:
.null: dq 0
.code:  dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x9A
        db 0xFA
        db 0x00
.data:  dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0x92
        db 0xFC
        db 0x00
gdt64_desc:   dw $ - gdt64 - 1
              dq gdt64

[bits 64]
section .text
long_mode:

    ; Set the Segment of Data into es, dx, gs, ss, and fs
    mov ax, 0x10
    mov es, ax
    mov ds, ax
    mov gs, ax
    mov ss, ax
    mov fs, ax

    [extern main]
    call main
.halt:
    hlt
    jmp .halt
