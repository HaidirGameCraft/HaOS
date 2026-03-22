.org 0x7C00
.code16

jmp short __boot_start
    nop

__boot_start:
    mov $0x7c00, %bp
    mov %bp, %sp

    movw %dl, (__drive_number)

__drive_number: .byte 0


.times 510-($-$$) .byte 0
.word 0xAA55
buffer:
