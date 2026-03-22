
extern __kernel_start
extern __kernel_end
section .header
header_start:
    db "KERNEL    ", 0
    dq __kernel_start
    dq __kernel_end
    dq __start

section .text
global __start
extern kernel_main
__start:
    ; bootstage info
    mov rax, rdi            ; Get the BootStage Address
    mov rsp, __stack_bottom

    mov rdi, rax
    call kernel_main

.halt:
    hlt
    jmp .halt

section .bss
__stack_top: resb 0x2000
__stack_bottom:
