#include <driver/video_driver.h>
#include <page.h>
#include <serial.h>
#include <font/vga-8x16.h>
#include <config.h>
#include <header.h>

static byte* framebuffer = NULL;
static word video_width = 0;
static word video_height = 0;
static word video_bpp = 0;

void    video_driver_init( ) {


    framebuffer = header.framebuffer;
    video_width = header.width & 0xFFFF;
    video_height = header.height & 0xFFFF;
    video_bpp = header.bytes_per_pixel / 8;

    serial_printf("Framebuffer: 0x%x\n", (qword) header.framebuffer );
    serial_printf("Width: %i, Height: %i\n", video_width, video_height );
    serial_printf("Bytes Per Pixel: %i\n", video_bpp );
}

void    video_driver_getsize( word* width, word* height ) {
    *width = video_width;
    *height = video_height;
}

void    video_driver_mapped() {
    serial_print("[Mapped Video Graphics] ...");
    page_mapvp( (qword) framebuffer, (qword) framebuffer, video_width * video_height * video_bpp , PAGE_PRESENT | PAGE_READWRITE );
    // framebuffer = (byte*) VIDEO_DRIVER_ADDRESS;
    // page_print();
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
