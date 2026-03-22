ENTRY(__start)

KERNEL_HIGH_HALF = 0xFFFFFFFF80100000;

SECTIONS
{
    . = KERNEL_HIGH_HALF;
    __kernel_start = .;
    .header : AT(ADDR(.header) - KERNEL_HIGH_HALF) {
        *(.header)
    }

    .text : AT(ADDR(.text) - KERNEL_HIGH_HALF) {
        *(.text)
    }

    .data : AT(ADDR(.data) - KERNEL_HIGH_HALF) ALIGN(4K) {
        *(.data)
    }

    .rodata : AT(ADDR(.rodata) - KERNEL_HIGH_HALF) ALIGN(4K) {
        *(.rodata)
    }

    .bss : AT(ADDR(.bss) - KERNEL_HIGH_HALF) ALIGN(0x1000) {
        *(COMMON)
        *(.bss)
    }
    . = ALIGN(0x1000);
    __heap_start = .;
    __kernel_end = .;
}
