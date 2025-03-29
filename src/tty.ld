ENTRY(main)
SECTIONS
{
    . = 0x00800000;
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
    . = ALIGN(0x1000);
    _heap_start = .;
    . += 0x100000;
    _heap_end = .;
}