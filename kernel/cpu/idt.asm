

%macro PUSHX64  0
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8

    push rsi 
    push rdi
    push rbp
    push rsp
    push rbx
    push rdx
    push rcx
    push rax

    mov rax, cr0
    push rax

    mov rax, cr2
    push rax

    mov rax, cr3
    push rax

    mov rax, cr4
    push rax

%endmacro

%macro POPX64   0
    add rsp, 32

    pop rax
    pop rcx
    pop rdx
    pop rbx
    pop rsp
    pop rbp
    pop rdi
    pop rsi 

    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
%endmacro



section  .text
global idt_install
idt_install:
    mov rax, rdi
    lidt [rax]
    ret

isr_stub:
    PUSHX64   ; Push all register

    mov rdi, rsp
    [extern isr_handle]
    call isr_handle

    POPX64    ; Pop all register

    add rsp, 16
    sti
    iretq        ; Interrupt return

isr_stub_halt:
    PUSHX64   ; Push all register

    mov rdi, rsp
    [extern isr_handle]
    call isr_handle

    POPX64    ; Pop all register
    add rsp, 16
.halt:
    hlt
    jmp .halt
    sti
    iretq        ; Interrupt return

irq_stub:
    PUSHX64

    mov rdi, rsp
    [extern irq_handle]
    call irq_handle

    POPX64
    add rsp, 16
    sti
    iretq

%macro isr_error 1
isr_%1:
    cli
    push qword %1
    jmp isr_stub_halt
%endmacro

%macro isr_no_error 1
isr_%1:
    cli
    push qword 0
    push qword %1
    jmp isr_stub
%endmacro

%macro irq__    2
irq_%1:
    cli
    push qword %1
    push qword %2
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
isr_error  8
isr_no_error  9
isr_error 10
isr_error 11
isr_error 12
isr_error 13
isr_error 14
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
    dq isr_0
    dq isr_1
    dq isr_2
    dq isr_3
    dq isr_4
    dq isr_5
    dq isr_6
    dq isr_7
    dq isr_8
    dq isr_9
    dq isr_10
    dq isr_11
    dq isr_12
    dq isr_13
    dq isr_14
    dq isr_15
    dq isr_16
    dq isr_17
    dq isr_18
    dq isr_19
    dq isr_20
    dq isr_21
    dq isr_22
    dq isr_23
    dq isr_24
    dq isr_25
    dq isr_26
    dq isr_27
    dq isr_28
    dq isr_29
    dq isr_30
    dq isr_31

    ; IRQ Interrupt Request Queue
    dq irq_0
    dq irq_1
    dq irq_2
    dq irq_3
    dq irq_4
    dq irq_5
    dq irq_6
    dq irq_7
    dq irq_8
    dq irq_9
    dq irq_10
    dq irq_11
    dq irq_12
    dq irq_13
    dq irq_14
    dq irq_15