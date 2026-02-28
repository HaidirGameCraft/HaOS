#include "term.h"
#include <colorcode.h>
#include <io.h>
#include <driver/video_driver.h>

struct term_text_cursor
{
    dword x;
    dword y;
    byte  attr;
};


dword text_color = 0;
dword background_color = 0;
int max_char_width = 0;
int max_char_height = 0;

struct term_text_cursor ttext_cursor;
struct term_text_cursor prev_ttext_cursor;

void term_drawChar( char c, dword color );

void term_init() {
    ttext_cursor.x = 0;
    ttext_cursor.y = 0;
    ttext_cursor.attr = 0;

    prev_ttext_cursor = ttext_cursor;
    text_color = R8G8B8(0xFF, 0xFF, 0xFF);
    background_color = R8G8B8(0x00, 0x00, 0x00);
}

void term_run() {
    word width, height;
    video_driver_getsize(&width, &height);

    max_char_width = (int)(width / 8);
    max_char_height = (int)(height / 16);

    while( true )
    {
        char key = getchar();
        if( key == 0 ) continue;
        else if( key == 0x08 )
        {

        } else {
            term_drawChar( key, text_color );
        }
        
    }
}

void term_drawChar( char c, dword color ) {
    if( c == 0x0A )
    {
        ttext_cursor.x = 0;
        ttext_cursor.y++;
        return;
    }

    int index = ttext_cursor.x + ttext_cursor.y * max_char_width;
    video_driver_drawChar( c, ttext_cursor.x * 8, ttext_cursor.y * 16, color );

    index++;
    ttext_cursor.x = (dword)(index % max_char_width );
    ttext_cursor.y = (dword)(index / max_char_width );
}