#pragma once

#include <type.h>
#include <config.h>

typedef struct {
    dword magic_number;
    dword version;
    word width_char;
    word height_char;
    word count_char;
    word zeroes;
    qword offset;
} __attribute__((packed)) font_bitmap_header_t;

static byte* vga_8x16_getbitmap( char __c )
{
    return &((byte*) DEFAULT_FONT_ADDRESS + sizeof( font_bitmap_header_t ) )[(int) __c * 16];
}
