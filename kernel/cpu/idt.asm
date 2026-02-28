

section  .text
global idt_install
idt_install:
    cli
    mov eax, dword [ esp + 4 ]
    lidt [eax]
    ret

isr_stub:
    pushf   ; Push the Eflags
    pusha   ; Push all register

    [extern isr_handle]
    call isr_handle

    popa    ; Pop all register
    popf    ; Pop the Eflags
    add esp, 8
    sti
    iret        ; Interrupt return

irq_stub:
    pushf
    pusha

    [extern irq_handle]
    call irq_handle

    popa
    popf
    add esp, 8
    sti
    iret

%macro isr_error 1
isr_%1:
    cli
    push dword %1
    jmp isr_stub
%endmacro

%macro isr_no_error 1
isr_%1:
    cli
    push dword 0
    push dword %1
    jmp isr_stub
%endmacro

%macro irq__    2
irq_%1:
    cli
    push dword %1
    push dword %2
    jmp irq_stub
%endmacro


isr_no_error  0
isr_no_error  1
isr_no_error  2
isr_no_error  3
isr_no_error  4
isr_no_error  5
isr_no_error  6
isr_no_error  7
isr_no_error  8
isr_no_error  9
isr_no_error 10
isr_no_error 11
isr_no_error 12
isr_no_error 13
isr_no_error 14
isr_no_error 15
isr_no_error 16
isr_no_error 17
isr_no_error 18
isr_no_error 19
isr_no_error 20
isr_no_error 21
isr_no_error 22
isr_no_error 23
isr_no_error 24
isr_no_error 25
isr_no_error 26
isr_no_error 27
isr_no_error 28
isr_no_error 29
isr_no_error 30
isr_no_error 31

irq__   0, 32
irq__   1, 33
irq__   2, 34
irq__   3, 35
irq__   4, 36
irq__   5, 37
irq__   6, 38
irq__   7, 39
irq__   8, 40
irq__   9, 41
irq__  10, 42
irq__  11, 43
irq__  12, 44
irq__  13, 45
irq__  14, 46
irq__  15, 47

section .data
global interrupt_x86
interrupt_x86:
    dd isr_0
    dd isr_1
    dd isr_2
    dd isr_3
    dd isr_4
    dd isr_5
    dd isr_6
    dd isr_7
    dd isr_8
    dd isr_9
    dd isr_10
    dd isr_11
    dd isr_12
    dd isr_13
    dd isr_14
    dd isr_15
    dd isr_16
    dd isr_17
    dd isr_18
    dd isr_19
    dd isr_20
    dd isr_21
    dd isr_22
    dd isr_23
    dd isr_24
    dd isr_25
    dd isr_26
    dd isr_27
    dd isr_28
    dd isr_29
    dd isr_30
    dd isr_31

    ; IRQ Interrupt Request Queue
    dd irq_0
    dd irq_1
    dd irq_2
    dd irq_3
    dd irq_4
    dd irq_5
    dd irq_6
    dd irq_7
    dd irq_8
    dd irq_9
    dd irq_10
    dd irq_11
    dd irq_12
    dd irq_13
    dd irq_14
    dd irq_15