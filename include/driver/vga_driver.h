#ifndef VGA_DRIVER
#define VGA_DRIVER

#include <stdint.h>
#include <string.h>

void vga_clearscreen(uint32_t color);
void vga_putc(uint8_t c);

#endif