section .header
[extern __kernel_start]
[extern __kernel_end]
    db "KERNEL    ", 0
    dd __kernel_start
    dd __kernel_end
    dd __start

section .text
__start:
    ; bootstage info
    mov eax, dword [esp + 4]
    mov esp, __stack_bottom

    push eax
    [extern kernel_main]
    call kernel_main

.halt:
    hlt
    jmp .halt

section .bss
align 4
__stack_top: resb 8196
__stack_bottom:
align 4