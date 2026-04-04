.section .text
.globl ata_set_lba
ata_set_lba:
    // rdi - ata bus
    // rsi - lba
    
    // [PORT + 6]: select hard disk following by higher lba and select MASTER(0x0E)|SLAVE(0x0F)
    mov %rdi, %rdx
    add $6, %rdx

    xor %rax, %rax
    movb $0x0E, %al
    shl $4, %al

    mov  %esi, %ebx
    shr  $0x18, %ebx
    and  $0x0F, %bl
    add %bl, %al

    outb %al, %dx

    // [PORT + 1]: set count sector
    mov %rdi, %rdx
    add $2, %rdx

    movb $1, %al
    outb %al, %dx

    // [PORT + 2]: set low LBA
    incw %dx
    movl %esi, %eax
    outb %al, %dx

    // [PORT + 3]: set mid LBA
    incw %dx
    shr  $0x08, %eax
    outb %al, %dx

    // [PORT + 4]: set high LBA
    incw %dx
    shr  $0x08, %eax
    outb %al, %dx

    ret
/*
.globl ata_read_disk:
ata_read_disk:
    push %rbp
    mov %rsp, %rbp

    subq $0x18, %rsp
    movq $rdi, %rax
    movq $rax, -0x10(%rbp)      // lba

    mov %rsi, %rax
    mov %rax, -0x08(%rbp)       // buffer
    
    mov %rbx, %rax
    mov %rax, -0x18(%rbp)       // size

    xor %rdx, %rdx
    mov $0x200, %rcx
    div %rcx

    cmp %edx, $0
    jne .end_len
    inc %rax
.end_len:
    mov %rax, -0x18(%rbp)       // len
    jmp .loop1cnd:
.loop1:

    mov -0x10(%rbp), %rax
    mov %rax, %rsi

    movq $0x1F0, %rdi
    call ata_set_lba        // set LBA

    // [PORT + 7]: set read command
    mov %di, %dx
    add $7, %dx
    movb $0x20, %al
    outb %al, %dx

.wait_until_busy_none:
    mov $0x1F0, %rdx
    add $7, %dx
    inb %dx, %al
    and $0x80, %al      // 0x80: BUSY_BIT
    test %al, %al
    jz .wait_until_busy_none

    sub %0x08, %rsp
    mov $0, -0x20(%rbp)     // i
    jmp .loop_read_disk_cnd
.loop_read_disk:
    
    mov $0x1F0, %rdx
    xor %rax, %rax
    inw %dx, %ax    // read disk

    mov -0x08(%rbp), %rbx
    movw %ax, (%rbx)

    addq $2, %rbx
    mov %rbx, -0x08(%rbp)
    
    mov -0x20(%rbp), %rax
    add $2, %rax
    mov %rax, -0x20(%rbp)
.loop_read_disk_cnd:
    mov -0x20(%rbp), %rax
    cmp %rax, 512
    jg .loop_read_disk
.loop_read_disk_end:
    add $0x08, %rsp

    mov -0x18(%rbp), %rax
    decq %rax
    mov %rax, -0x18(%rbp)
.loop1cnd:
    mov -0x18(%rbp), %rax
    cmp %rax, $0
    jl .loop1
.loop1end:

    mov %rbp, %rsp
    pop %rbp
    ret
*/
