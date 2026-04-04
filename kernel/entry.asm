
extern __kernel_start
extern __kernel_end
section .header
global header
header_start:
header:
    db "KERNEL    ", 0
    dq __kernel_start
    dq __kernel_end
    dq __start

    dq 0            ; Font Data Start

    ; Graphics Information
    dd 0            ; Width
    dd 0            ; Height
    db 0            ; Bytes Per pixel
    dq 0            ; Framebuffer
    dd 0            ; Size of Framebuffer

section .text
global __start
extern kernel_main
extern serial_connect
extern serial_printf
__start:
    ; bootstage info
    mov rax, rdi            ; Get the BootStage Address
    mov rsp, KERNEL_STACK_BOTTOM

    call kernel_main

.halt:
    hlt
    jmp .halt

section .data
test_message: db "hello, World", 0xa, 0x0

section .bss
global KERNEL_STACK_TOP
global KERNEL_STACK_BOTTOM
KERNEL_STACK_START: resb 0x4000
KERNEL_STACK_BOTTOM:
