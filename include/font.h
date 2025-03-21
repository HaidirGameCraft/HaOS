#ifndef FONT_HEADER
#define FONT_HEADER

#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
} C_PACKED CharBitmap_Header;

typedef struct {
    char char__;
    uint8_t bitmap[16];
} C_PACKED CharBitmap_Char;

void Font_Init(const char* font_file);
void Font_InitAddress(const char* font_file, addr_t font_address);
uint8_t* Font_GetBitmap(char __c);

#endif