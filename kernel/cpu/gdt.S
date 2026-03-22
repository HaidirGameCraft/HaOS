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
    ret

.globl tss_install
tss_install:
    movq %rdi, %rax
    ltr (%rax)
    ret

