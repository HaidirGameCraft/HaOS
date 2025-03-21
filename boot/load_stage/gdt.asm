%ifndef GDT_HEADER
%define GDT_HEADER
gdt_start:
.null:
    dq 0
.code:
    dw 0xFFFF
    dw 0x0000
    db 0
    db 0x9A
    db 0xFC
    db 0
.data:
    dw 0xFFFF
    dw 0x0000
    db 0
    db 0x92
    db 0xFC
    db 0
gdt_desc:
    dw gdt_desc - gdt_start - 1
    dd gdt_start
%endif