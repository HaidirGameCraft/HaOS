ENTRY(__start)
SECTIONS
{
    . = 0xC0100000;
    __kernel_start = .;
    .header : {
        *(.header)
    }

    .text : ALIGN(0x1000) {
        *(.text)
    }

    .data : ALIGN(0x1000) {
        *(.data)
    }

    .rodata : ALIGN(0x1000) {
        *(.rodata)
    }

    .bss : ALIGN(0x1000) {
        *(COMMON)
        *(.bss)
    }
    __heap_start = .;
    __kernel_end = .;
}