.section .text

.extern print
.extern __tprint
.extern task_current
.extern task_next
.extern kernel_task

.globl switch_task

switch_task:

    // rdi - cpu_register_t cpu
    mov %rdi, %rsp
    // add $0x20, %rsp     // removing cr4, cr3, cr2, cr0

   // Check next is null or not
    leaq  (task_current), %rax
    movq  (%rax), %rax            // rax == task_current
    movq  0x28(%rax), %rax       // rax = task_current->next
    cmp $0, %rax                 // rax == 0
    je .set_to_main_task
    movq %rax, %rdi
    leaq (task_next), %rax
    movq %rdi, (%rax)            // task_next = task_current->next

    jmp .switch
.set_to_main_task:
    lea (task_next), %rdi
    lea (kernel_task), %rax
    movq %rax, (%rdi)           // task_next = &kernel_task
.switch:

    // Switch task ( Save Current Task )
    movq (task_current), %rax
    movq %rsp, 0x10(%rax)           // saving rsp to stack

    // Now load next task
    movq (task_next), %rax
    movq 0x10(%rax), %rsp           // restore next rsp

    // now swap next_task to current
    movq %rax, (task_current)   // task_current = task_next

    // pop all
    add $0x20, %rsp

    pop %rax
    pop %rcx
    pop %rdx
    pop %rbx
    add $0x08, %rsp             // ignore old_rsp
    pop %rbp
    pop %rdi
    pop %rsi

    pop %r8
    pop %r9
    pop %r10
    pop %r11
    pop %r12
    pop %r13
    pop %r14
    pop %r15

    add $0x10, %rsp
    iretq

.globl task_create
.extern task_create_impl
task_create:
    push %rbp
    mov %rsp, %rbp

    // Arguments
    cmp $0, %rsi
    je 1f
    push %r9
    push %r8
    push %rdx
    push %rcx
    push %rsi
    push %rdi
    jmp 2f
1:
    push $0
    push $0
    push $0
    push $0
    push %rsi
    push %rdi
2:
    mov %rsp, %rdi
    call task_create_impl
    add $48, %rsp

    mov %rbp, %rsp
    pop %rbp
    ret

.globl switch_next_task_imme
switch_next_task_imme:
    movq (task_next), %rax
    mov 0x10(%rax), %rsp            // load stack address
    movq %rax, (task_current)       // set task_next as task_current

    add $0x20, %rsp             // skipping cr4, cr3, cr2 and cr0

    pop %rax
    pop %rcx
    pop %rdx
    pop %rbx
    add $0x08, %rsp             // ignore old_rsp
    pop %rbp
    pop %rdi
    pop %rsi

    pop %r8
    pop %r9
    pop %r10
    pop %r11
    pop %r12
    pop %r13
    pop %r14
    pop %r15

    add $0x10, %rsp
    iretq

.globl task_remove
.extern page_umapv
.extern free_alloc
task_remove:
    lea (task_stack), %rax
    mov %rax, %rsp
    mov %rsp, %rbp
    sub $0x18, %rsp
    
    mov (task_current), %rax
    mov %rax, -0x18(%rbp)         // struct task_info_t* target
    
    lea (kernel_task), %rax
    mov %rax, -0x10(%rbp)         // struct task_info_t* prev

    mov -0x18(%rbp), %rax
    mov  0x28(%rax), %rax
    mov  %rax, -0x8(%rbp)         // struct_task_info_t* next

    jmp  .loop_get_prev_cnd
.loop_get_prev:
    mov -0x10(%rbp), %rax
    mov 0x28(%rax), %rax
    mov %rax, -0x10(%rbp)
    cmp $0, %rax
    jne .loop_get_prev_cnd

    lea (task_message_error_NULL), %rax
    mov %rax, %rdi
    call printf

.halt:
    hlt
    jmp .halt
.loop_get_prev_cnd:
    mov -0x18(%rbp), %rax
    mov -0x10(%rbp), %rbx
    mov 0x28(%rbx), %rbx
    cmp %rax, %rbx
    jne .loop_get_prev
.loop_get_prev_end:

    // prev->next = next
    mov -0x10(%rbp), %rax
    mov -0x8(%rbp), %rbx
    mov %rbx, 0x28(%rax)
    cmp $0, %rbx
    jne .set_task
    lea (kernel_task), %rbx
.set_task:
    // swap task_next
    lea (task_next), %rax
    mov %rbx, (%rax)   // task_next = next

    mov -0x18(%rbp), %rax
    mov 0x08(%rax), %rdi
    mov $0x1000, %rsi
    call page_umapv

    mov -0x18(%rbp), %rax
    mov %rax, %rdi
    call free_alloc

    jmp switch_next_task_imme
    nop


.section .data
task_switch_msg: .asciz "Switch Task"
task_message_exit: .asciz "Task exit with Signal Code (%x)\n"
task_message_error_NULL: .asciz "Abort: Prev Task is NULL"

.section .bss
.globl task_stack
.skip 0x1000
task_stack: