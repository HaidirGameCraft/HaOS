#include <type.h>
#include <config.h>
#include <header.h>
#include "font.h"


byte* vga_8x16_getbitmap( char __c )
{
    return &((byte*) header.font_data + sizeof( font_bitmap_header_t ) )[(int) __c * 16];
}
