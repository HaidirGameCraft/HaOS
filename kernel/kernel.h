#ifndef KERNEL
#define KERNEL

#include <boot.h>

// void kernel_main(uint16_t w, uint16_t h, uint8_t bpp, uint32_t a);
int exec(const char* path);

#endif