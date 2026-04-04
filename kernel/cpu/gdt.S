.section .text
.global gdt_install
gdt_install:
    cli
    movq %rdi, %rax
    lgdt (%rax)

    leaq  (.flush), %rax
    pushq $0x08
    pushq %rax
    retfq
.flush:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %ss
    mov %es, %ax
    ret

.globl tss_install
tss_install:
    // di - selector segmentation
    mov %di, %ax
    ltr %ax
    ret

