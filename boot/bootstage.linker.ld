SECTIONS
{
    . = 0x8000;
    __BOOTSTAGE_START = .;
    .header : {
        *(.header)
    }

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .rodata : {
        *(.rodata)
    }

    .bss : {
        *(COMMON)
        *(.bss)
    }
    __heap_start = .;
    . += 0x2000;
    __heap_end = .;
    __BOOTSTAGE_END = .;
}