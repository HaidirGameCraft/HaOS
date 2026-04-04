[bits 32]

section .header
[extern __BOOTSTAGE_START]
[extern __BOOTSTAGE_END]
identifier: db "BOOTSTAGE", 0
bootstage_start: dd __BOOTSTAGE_START
bootstage_end: dd __BOOTSTAGE_END
entry_bootstage_start: dd __start

[extern serial_init]
[extern asm_serial_print]
[extern vesaInfo]
[extern vesaModeInfo]

%define COM1 0x3F8
section .text
__start:
    mov esp, stack_bottom
    mov ebp, esp

    ; Copy and clear address
    mov esi, 0x2000
    mov edi, vesaInfo
    mov ecx, 0 
.copy_vesa_info:
    mov eax, dword [esi]
    mov dword [edi], eax
    mov dword [esi], 0
    add esi, 4
    add edi, 4
    inc ecx
    cmp ecx, 128
    jl .copy_vesa_info

    mov esi, 0x3000
    mov edi, vesaModeInfo
    mov ecx, 0
.copy_vesa_mode_info:
    mov eax, dword [esi]
    mov dword [edi], eax
    mov dword [esi], 0
    add esi, 4
    add edi, 4
    inc ecx
    cmp ecx, 64
    jl .copy_vesa_mode_info

    ;mov bx, COM1
    ;call serial_init

    call check_cpuid_support
    call check_longmode_support


;;    mov ax, 0x0F00 | 'G'
;;    mov word [0xb8000], ax

    mov esi, test_message
;    call print_msg
    jmp setup_to_longmode


    ; [extern main]
    ; call main
halt:
    hlt
    jmp halt

; Checking the Long Mode Support and CPUID
global check_cpuid_support
check_cpuid_support:
    pushfd
    pop eax

    mov ecx, eax ; save the eflags into different register
    or eax, 0x200000 ; set the ID bits into eflags

    push eax
    popfd

    ; get and check whether the ID bits is remains unchanges or not
    pushfd
    pop eax

    xor eax, ecx
    push ecx
    popfd

    and eax, 0x200000
    jnz .cpuid_support
    mov eax, 1

    mov eax, ((0x0F00 | 'C') << 16) | (0x0F00 | 'U' )
    mov dword [0xb8000], eax
    mov esi, message_cpuid_unsupport
    call asm_serial_print

    jmp halt
.cpuid_support:
    mov eax, 0

    mov eax, ((0x0F00 | 'C') << 16) | (0x0F00 | 'S' )
    mov dword [0xb8000], eax
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

    mov eax, ((0x0F00 | 'L') << 16) | (0x0F00 | 'U' )
    mov dword [0xb8004], eax
    

;    mov esi, message_longmode_unsupport
;    call print_msg
    ;call asm_serial_print

    mov eax, 1
    jmp halt
.long_mode_support:
    mov eax, ((0x0F00 | 'L') << 16) | (0x0F00 | 'S' )
    mov dword [0xb8004], eax
    mov esi, message_longmode_support
;    call print_msg
    ;call asm_serial_print

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

    mov esi, PAGE_BITMAP_ADDRESS
.loop_clear:
    push esi
    call clear_table
    pop esi

    add esi, 0x1000
    cmp esi, FRAME_PT_ADDRESS
    jle .loop_clear


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
    mov dword [PDT_ADDRESS + 4088], eax

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

    mov eax, ((0x0F00 | 'E') << 16) | (0x0F00 | 'P' )
    mov dword [0xb8008], eax
    ;mov esi, test_message
    ;call print_msg

    ; Setup GDT for 64bits
    cli
    lgdt [gdt64_desc]

    jmp 0x8:long_mode

.halt:
    hlt
    jmp .halt

clear_table:
    ; esi - address of table
    mov ecx, 0
.loop:
    mov dword [esi], 0
    add esi, 4
    inc ecx
    cmp ecx, 0x80
    jl .loop

    ret

print_msg:
    push esi
    ; esi - message buffer
.disp:

    ; set index
    xor eax, eax
    mov ax, word [__text_rows]
    mov cx, 25
    mul cx
    add ax, word [__text_cols]
    mov cx, 2
    mul cx

    mov ebx, eax
    add ebx, 0xb8000

    push eax
    mov al, byte [esi]
    inc esi
    cmp al, 0xa
    jne .display
    ; next line
    mov word [__text_cols], 0
    add word [__text_rows], 1
    jmp .disp
.display
    cmp al, 0
    je .done
    mov ah, 0x0F
    mov word [ebx], ax
    pop eax

    add eax, 2
    div cx

    xor dx, dx
    mov cx, 25
    div cx
    
    mov word [__text_cols], dx
    mov word [__text_rows], ax

    je .disp
.done:
    pop esi
    ret

section .rodata
test_message: db "Testing Display Message", 0xa, 0

section .data
__text_rows: dw 0x0000
__text_cols: dw 0x0000
gdt64:
.null:  dq 0
.code:  dd 0x0000FFFF
        dd 0x00AF9A00
.data:  dd 0x0000FFFF
        dd 0x00CF9200
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

    ;mov eax, ((0x0F00 | 'L') << 16) | ((0x0F00 | 'M'))
    ;mov dword [0xb80012], eax

    [extern main]
    ;call main
.halt:
    hlt
    jmp .halt
