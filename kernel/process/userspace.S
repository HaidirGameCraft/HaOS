

.section .text
.globl turn_to_userspace
.extern tss_entry
turn_to_userspace:
    cli

    // set the Segmentation into User Data segment
    movw $0x23, %ax
    mov %ax, %es
    mov %ax, %ds 
    mov %ax, %fs
    mov %ax, %gs

    // We need to jump to userspace throught interrupt style iretq
    // following by:
    push   $0x23
    
    mov %rsi, %rax
    push %rax       // push  new stack

    push   $0x202
    push   $0x1B  // pushing segmentation to cs on stack
    
    mov %rdi, %rax
    pushq %rax
    iretq        // Now, We entering userspace

.globl exec_sysenter
exec_sysenter:
    movw $0x23, %ax
    mov %ax, %es
    mov %ax, %ds
    mov %ax, %fs
    mov %ax, %gs

    xor %rdx, %rdx
    xor %rax, %rax
    mov $0x08, %ax

    xor %rcx, %rcx
    movl $0x000000174, %ecx
    wrmsr

    // Entering Ring 3
    mov %rdi, %rdx
    mov %rsp, %rcx
    sysexit




