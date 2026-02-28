#pragma once

#include <type.h>
#include <header.h>


void    video_driver_init( bootstage_info_t* bootstage_info );
void    video_driver_mapped();
void    video_driver_putPixel( int x, int y, dword color );
void    video_driver_clearScreen( dword color );
void    video_driver_drawChar( char c, dword x, dword y, dword color );
void    video_driver_getsize( word* width, word* height );