#include <driver/video_driver.h>
#include <page.h>
#include <serial.h>
#include <font/vga-8x16.h>

byte* framebuffer = NULL;
word video_width = 0;
word video_height = 0;
word video_bpp = 0;

void    video_driver_init( bootstage_info_t* bootstage_info ) {
    framebuffer = (byte*) bootstage_info->framebuffer;
    video_width = bootstage_info->width;
    video_height = bootstage_info->height;
    video_bpp = bootstage_info->bytes_per_pixel / 8;

    serial_printf("Framebuffer: 0x%x\n", framebuffer );
    serial_printf("Width: %i, Height: %i\n", video_width, video_height );
    serial_printf("Bytes Per Pixel: %i\n", video_bpp );
}

void    video_driver_getsize( word* width, word* height ) {
    *width = video_width;
    *height = video_height;
}

void    video_driver_mapped() {
    serial_print("[Mapped Video Graphics] ...");
    page_mapvp( (dword) framebuffer, (dword) framebuffer, video_width * video_height * video_bpp );
    serial_print(" [DONE]\n");
}

void    video_driver_putPixel( int x, int y, dword color ) {
    int index = (x + y * video_width) * video_bpp;
    for( int i = 0; i < video_bpp; i++ )
        framebuffer[index + i] = ( color >> (i * 8) ) & 0xFF;
}
void    video_driver_clearScreen( dword color ) {
    for( int i = 0; i < video_height; i++ )
        for( int j = 0; j < video_width; j++ )
            video_driver_putPixel(j, i, color );
}

void    video_driver_drawChar( char c, dword x, dword y, dword color ) {
    byte* row_arr = (byte*) vga_8x16_getbitmap( c );
    for( int i = 0; i < 16; i++ )
    {
        byte b = row_arr[i];
        for( int j = 0; j < 8; j++ )
        {
            if( ( b >> (7 - j) & 1 ) == 1 )
                video_driver_putPixel(x + j, y + i, color);
        }
    }
}