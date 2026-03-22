section .text
global idt_install
idt_install:
    mov rax, rdi
    lidt [rax]
    ret

isr_stub:
    cli
    pushfq
    pushaq

    [extern isr_handle]
    call isr_handle

    popaq
    popfq
    add rsp, 8
    sti
    iretq


global isr_page_fault
isr_page_fault:
    cli
    push qword 14
    jmp isr_stub

global isr_divide_zero
isr_divide_zero:
    push qword 0
    push qword 0
    jmp isr_stub