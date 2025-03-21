#include <font.h>

#include <fs/fat.h>
#include <string.h>
#include <driver/video_driver.h>
#include <memory.h>

#define FIX_FONT_ADDRESS 0x000A0000
CharBitmap_Header font_header;
CharBitmap_Char* font_chars;
void Font_Init(const char* font_file) {
    FILE* fontfile = find_file(font_file);
    if( fontfile->flags != 0 )
    {
        read_file(fontfile, (void*) FIX_FONT_ADDRESS);
        memcpy(&font_header, (void*) FIX_FONT_ADDRESS, sizeof( CharBitmap_Header ));

        font_chars = (CharBitmap_Char*)( FIX_FONT_ADDRESS + sizeof( CharBitmap_Header ));
    } else {
        VDrider_ClearScreen(0x00FFFFFF);
    }
}

void Font_InitAddress(const char* font_file, addr_t font_address) {
    FILE* fontfile = find_file(font_file);
    if( fontfile->flags != 0 )
    {
        read_file(fontfile, (void*) font_address);
        memcpy(&font_header, (void*) font_address, sizeof( CharBitmap_Header ));

        font_chars = (CharBitmap_Char*) (font_address + sizeof( CharBitmap_Header )) ;
    } else {
        VDrider_ClearScreen(0x00FFFFFF);
    }
}

uint8_t* Font_GetBitmap(char __c) {
    return font_chars[__c].bitmap;
}