#pragma once

#include <type.h>
#include <header.h>

void    video_driver_init( 
    #ifdef BOOTSTAGE64
        bootstage_info_t* bootstage_info
    #endif
);
void    video_driver_mapped();
void    video_driver_putPixel( int x, int y, dword color );
void    video_driver_clearScreen( dword color );
void    video_driver_drawChar( char c, dword x, dword y, dword color );
void    video_driver_getsize( word* width, word* height );
qword   video_driver_getframebuffer();