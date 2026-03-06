#pragma once

#define PAGE_BITMAP_ADDRESS     0x1000
#define PAGE_BITMAP_SIZE        0x1000

#define MAX_ARGS                64

#define PAGEBITMAP_ADDRESS      0x1000
#define PAGEDIR_ADDRESS         0x2000
#define PAGEDIR_FIRSTTABLE      0x3000
#define PAGEDIR_FRAMETABLE      0x4000
#define PAGEDIR_HALFKERNEL      0x5000

extern void __kernel_start();
extern void __kernel_end();
extern void __heap_start();