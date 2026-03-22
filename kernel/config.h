#pragma once

#define PAGE_BITMAP_ADDRESS         0x1000
#define PAGE_BITMAP_SIZE            0x1000

#define MAX_ARGS                    64

#define PAGE_BITMAP_ADDRESS         0x1000
#define PMLT4_ADDRESS               0x2000
#define PDPT_ADDRESS                0x3000
#define PDT_ADDRESS                 0x4000
#define FIRST_PT_ADDRESS            0x5000
#define FRAME_PT_ADDRESS            0x6000

#define VIDEO_DRIVER_ADDRESS        0xFFFFFFFFFD000000
#define KERNEL_HIGHER_HALF_KKERNEL  0xFFFFFFFF80000000

extern void __kernel_start();
extern void __kernel_end();
extern void __heap_start();
