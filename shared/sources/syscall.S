.section .text
.globl read
read:
    mov $0, %rax
    int $0x80
    ret

.globl open
open:
    mov $2, %rax
    int $0x80
    ret

.globl close
close:
    mov $3, %rax
    int $0x80
    ret

.globl put_pixel
put_pixel:
    mov $4, %rax
    int $0x80
    ret

.globl putc
putc:
    mov $5, %rax
    int $0x80
    ret

.globl exit
exit:
    mov $6, %rax
    int $0x08
    ret

