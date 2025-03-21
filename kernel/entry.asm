%ifndef ENTRY_DEFINE
%define ENTRY_DEFINE
[extern Page_Initialize]
[extern memory_initialize]
[extern gdt_initialize]
[extern idt_initialize]
[extern Keyboard_Initialize]
[extern kernel_main]
[extern _kernel]
__start:
    mov ebp, stack_bottom
    mov esp, ebp
.load:

    push eax
    call kernel_main
    hlt
    jmp $

    call _kernel

.halt:
    cli
    hlt
    jmp .halt

section .bss
stack_top:
    resb 1024 * 8
stack_bottom:

%endif