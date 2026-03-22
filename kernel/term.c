#include "term.h"
#include <colorcode.h>
#include <io.h>
#include <string.h>
#include <driver/video_driver.h>
#include <alloc.h>
#include <page.h>
#include <serial.h>

#include <config.h>
#include <elf.h>

struct term_text_cursor
{
    dword x;
    dword y;
    byte  attr;
};


static dword text_color = 0;
static dword background_color = 0;
static dword max_char_width = 0;
static dword max_char_height = 0;
static char* logs = NULL;
static dword index_logs = 0;

struct term_text_cursor ttext_cursor;
struct term_text_cursor prev_ttext_cursor;

void term_drawChar( char c, dword color );
void term_scrollup();
void term_exec( char* cmd );

void term_init() {
    ttext_cursor.x = 0;
    ttext_cursor.y = 0;
    ttext_cursor.attr = 0;

    prev_ttext_cursor = ttext_cursor;
    text_color = R8G8B8(0xFF, 0xFF, 0xFF);
    background_color = R8G8B8(0x00, 0x00, 0x00);

    word width, height;
    video_driver_getsize(&width, &height);
    max_char_width = (int)(width / 8);
    max_char_height = (int)(height / 16);
}

void term_run() {
    word width, height;
    video_driver_getsize(&width, &height);

    
    logs = (char*) new_alloc( 1024 );
    index_logs = 0;


    term_drawChar('>', text_color);
    while( 1 )
    {
        char key = getchar();
        if( key == 0 ) continue;
        else if( key == 0x08 )
        {
            if( index_logs <= 0 )
                continue;
            char _c = logs[index_logs - 1];
            logs[index_logs - 1] = 0;
            index_logs--;
            int index = ttext_cursor.x + ttext_cursor.y * max_char_width;
            index--;

            ttext_cursor.x = (dword)(index % max_char_width );
            ttext_cursor.y = (dword)(index / max_char_width );
            video_driver_drawChar( _c, ttext_cursor.x * 8, ttext_cursor.y * 16, R8G8B8(0, 0, 0) );
        }
        else if ( key == '\n' )
        {
            
            ttext_cursor.x = 0;
            ttext_cursor.y++;
            term_scrollup();
            
            term_exec( logs );
            index_logs = 0;
            logs[ index_logs ] = 0;

            term_drawChar('>', text_color);
        } 
        else {
            if( key >= 0x20 && key < 127) {
                logs[ index_logs++ ] = key;
                logs[ index_logs ] = 0;
                term_drawChar( key, text_color );
            }
        }
        
    }
}

void term_drawChar( char c, dword color ) {
    if( c == 0x0A )
    {
        ttext_cursor.x = 0;
        ttext_cursor.y++;
        term_scrollup();
        return;
    }

    int index = ttext_cursor.x + ttext_cursor.y * max_char_width;
    video_driver_drawChar( c, ttext_cursor.x * 8, ttext_cursor.y * 16, color );

    index++;
    ttext_cursor.x = (dword)(index % max_char_width );
    ttext_cursor.y = (dword)(index / max_char_width );
    term_scrollup();
}

void term_putc(char c) {
    term_drawChar( c , text_color );
}

void term_scrollup() {

    // skip if the y cursor is lower than max of height
    if( ttext_cursor.y < max_char_height )
        return;

    byte* framebuffer = ( byte* ) video_driver_getframebuffer();
    word width, height;
    video_driver_getsize(&width, &height);


    int size = width * 16 * 3;
    for( int i = 0; i < max_char_height - 1; i++ )
    {
        // source address = framebuffer + ( width screen * height of char * bytes per pixel ) * i
        void* src_addr = (void*) &framebuffer[ size * (i + 1) ];
        // destination address
        void* dest_addr = (void*) &framebuffer[ size * i ];

        // copy src pixel into dest pixel
        memcopy( (void*) dest_addr, (void*) src_addr, size );
    }

    // clear pixels at last line
    void* dest_addr = ( void* ) &framebuffer[ ( width * 16 * 3 ) * ( max_char_height - 1) ];
    memzero( (void*) dest_addr, width * 16 * 3 );
    ttext_cursor.y = max_char_height - 1;

    int index = ttext_cursor.x + ttext_cursor.y * max_char_width;
    ttext_cursor.x = (dword)(index % max_char_width );
    ttext_cursor.y = (dword)(index / max_char_width );
}

void term_exec( char* cmd ) {
    // split the cmd into args
    char* args[ MAX_ARGS ];
    int index = 0;
    int len = strsize( cmd );
    int argc = 0;
    char* start_addr = cmd;
    while( index <= len )
    {
        if( cmd[index] == ' ' || cmd[index] == 0 )
        {
            args[argc] = start_addr;
            cmd[index] = 0;
            index++;
            start_addr = &cmd[index];
            argc++;
            continue;
        }
        index++;
    }

    

    if( argc > 0 )
    {
        if( strcmp( args[0], "HELP" ) == 0 ) {
            printf("Tools/Command HaOS:\n");
            printf("HELP -  displaying tools/command\n");
            printf("CLEAR - clear text/screen of terminal\n");
            printf("EXEC <filename> - execute ELF file target\n");
            printf("SETTEXTCOLOR <r8> <g8> <b8> - change text color\n");
            printf("MEMSIZE -[K]B - show the size of memory use\n");
        }
        else if( strcmp(args[0], "CLEAR") == 0 ) {
            video_driver_clearScreen( R8G8B8(0, 0, 0 ) );
            ttext_cursor.x = 0;
            ttext_cursor.y = 0;
        }
        else if ( strcmp(args[0], "EXEC") == 0 ) {
            if( argc != 2 )
            {
                printf("EXEC <filename>");
                return;
            }

            elf64_load( args[1] );
        }
        else if ( strcmp(args[0], "SETTEXTCOLOR") == 0 )
        {
            if( argc != 4 )
            {
                printf("Text Color should be have r, g, and b value\n");
                return;
            }
            byte r = (byte) strint( args[1] );
            byte g = (byte) strint( args[2] );
            byte b = (byte) strint( args[3] );

            text_color = R8G8B8(r, g, b);
        } else if ( strcmp("MEMSIZE", args[0]) == 0 )
        {
            qword mem_use = page_getSizeMemoryUse();
            if( argc > 1 && strcmp(args[1], "-KB") == 0 ) {
                qword n = (qword)( mem_use / 1024 );
                printf("Memory Size Use: %i kb \n", n );
            } else {
                printf("Memory Size Use: %i bytes \n", mem_use);
            }
        } else {
            printf("%s: is not an COMMAND\n", args[0] );
        }
    }
}
