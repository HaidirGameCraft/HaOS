.section .text
.globl page_enable

// Page Enable
.equ CR0_PE,             0x80000000
// Protected Mode Enable 
.equ CR0_PME,            0x01

page_enable:
    mov %rdi, %rax
    mov %rax, %cr3      // set the address of Page Directory/PMLT4

    // Enable paging ( optional )
    mov %cr0, %rax
    orl $CR0_PE, %eax
    orl $CR0_PME, %eax
    mov %rax, %cr0

    ret
