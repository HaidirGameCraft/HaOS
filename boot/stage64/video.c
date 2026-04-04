#include <type.h>
#define BOOTSTAGE64
#include <driver/video_driver.h>
#include <config.h>
#include <page.h>
#include "font.h"
#include "io.h"


static byte* framebuffer = NULL;
static word video_width = 0;
static word video_height = 0;
static word video_bpp = 0;

void    video_driver_init( bootstage_info_t* bootstage_info ) {


    framebuffer = (byte*) bootstage_info->framebuffer;
    video_width = bootstage_info->width;
    video_height = bootstage_info->height;
    video_bpp = bootstage_info->bytes_per_pixel / 8;

    printf("Framebuffer: 0x%x\n", (qword) bootstage_info->framebuffer );
    printf("Width: %i, Height: %i\n", video_width, video_height );
    printf("Bytes Per Pixel: %i\n", video_bpp );
}

void    video_driver_getsize( word* width, word* height ) {
    *width = video_width;
    *height = video_height;
}

void    video_driver_mapped() {
    print("[Mapped Video Graphics] ...");
    page_mapvp( (qword) framebuffer, (qword) framebuffer, video_width * video_height * video_bpp, PAGE_PRESENT | PAGE_READWRITE);
}

void    video_driver_putPixel( int x, int y, dword color ) {
    qword index = (x + y * video_width) * video_bpp;

    byte r8 = color & 0xFF;
    byte g8 = ( color >> 8 ) & 0xFF;
    byte b8 = ( color >> 16 ) & 0xFF;
    __asm__ volatile("movq %0, %%rax" :: "r"(index));
    __asm__ volatile("movq %rax, %rsi");
    __asm__ volatile("addq %0, %%rsi" :: "r"( framebuffer ));
    __asm__ volatile("movb %0,(%%rsi)" :: "r"( b8 ) );
    __asm__ volatile("movb %0, 0x01(%%rsi)" :: "r"(g8) );
    __asm__ volatile("movb %0, 0x02(%%rsi)" :: "r"(r8) );
}
void    video_driver_clearScreen( dword color ) {
    for( dword i = 0; i < video_height; i++ )
        for( dword j = 0; j < video_width; j++ )
            video_driver_putPixel(j, i, color );
}

void    video_driver_drawChar( char c, dword x, dword y, dword color ) {
    byte* row_arr = (byte*) vga_8x16_getbitmap( c );
    for( dword i = 0; i < 16; i++ )
    {
        byte b = row_arr[i];
        for( dword j = 0; j < 8; j++ )
        {
            if( ( b >> (7 - j) & 1 ) == 1 )
                video_driver_putPixel(x + j, y + i, color);
        }
    }
}

qword   video_driver_getframebuffer() {
    return (qword) framebuffer;
}
