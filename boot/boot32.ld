SECTIONS
{
    . = 0x7E00;
    .header : {
        *(.header)
    }
    .text : {
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(COMMON)
        *(.bss)
    }
}
