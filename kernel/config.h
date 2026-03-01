#pragma once

#define PAGE_BITMAP_ADDRESS     0x1000
#define PAGE_BITMAP_SIZE        0x1000

#define MAX_ARGS                64

extern void __kernel_start();
extern void __kernel_end();
extern void __heap_start();