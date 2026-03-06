section .text
global idt_install
idt_install:
    mov eax, dword [esp + 4]
    lidt [eax]
    ret

isr_stub:
    cli
    pushf
    pusha

    [extern isr_handle]
    call isr_handle

    popa
    popf
    add esp, 8
    sti
    iret


global isr_page_fault
isr_page_fault:
    push dword 14
    jmp isr_stub