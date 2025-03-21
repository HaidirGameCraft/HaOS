section .text
[extern isr_handler]
[extern irq_handler]

%macro ISR_STUB 1
global ISR%1
ISR%1:
    cli
    push byte 0
    push byte %1
    pusha
    call isr_handler
    popa
    add esp, 4
    sti
    iret
%endmacro

%macro IRQ_STUB 2
global IRQ%2
IRQ%2:
    cli
    push byte %2
    push byte %1
    pusha
    call irq_handler
    popa
    add esp, 8
    sti
    iret
%endmacro

ISR_STUB 0
ISR_STUB 1
ISR_STUB 2
ISR_STUB 3
ISR_STUB 4
ISR_STUB 5
ISR_STUB 6
ISR_STUB 7
ISR_STUB 8
ISR_STUB 9
ISR_STUB 10
ISR_STUB 11
ISR_STUB 12
ISR_STUB 13
ISR_STUB 14
ISR_STUB 15
ISR_STUB 16
ISR_STUB 17
ISR_STUB 18
ISR_STUB 19
ISR_STUB 20
ISR_STUB 21
ISR_STUB 22
ISR_STUB 23
ISR_STUB 24
ISR_STUB 25
ISR_STUB 26
ISR_STUB 27
ISR_STUB 28
ISR_STUB 29
ISR_STUB 30
ISR_STUB 31

IRQ_STUB 32, 0
IRQ_STUB 33, 1
IRQ_STUB 34, 2
IRQ_STUB 35, 3
IRQ_STUB 36, 4
IRQ_STUB 37, 5
IRQ_STUB 38, 6
IRQ_STUB 39, 7
IRQ_STUB 40, 8
IRQ_STUB 41, 9
IRQ_STUB 42, 10
IRQ_STUB 43, 11
IRQ_STUB 44, 12
IRQ_STUB 45, 13
IRQ_STUB 46, 14
IRQ_STUB 47, 15

global SYSCALL
SYSCALL:
    cli
    [extern syscall_handler]
    call syscall_handler
    sti
    iret