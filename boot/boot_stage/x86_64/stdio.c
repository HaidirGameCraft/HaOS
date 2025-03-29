#include <stdio.h>
#include <driver/vga_driver.h>
#include <driver/video_driver.h>

void printf(const char* format, ...) {
    int index = 0;
    while( format[index] != 0 )
    {
        VDriver_PutC( format[index] );
        index++;
    }
}

void printh(uint32_t value, size_t size_byte) {
    uint32_t val = value;
    char* words_hex = "0123456789ABCDEF";
    for(int i = 0; i < size_byte; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            char c = words_hex[((value >> ((size_byte - 1) - i) * 8)) >> (1 - j) * 4 & 0x0F];
            VDriver_PutC(c);
        }
    }
}