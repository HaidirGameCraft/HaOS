#include <syscall.h>
#include <stdint.h>
#include <driver/video_driver.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <cpu/ports.h>

void command(char* text_buffer, int* index_buffer);
char* terminal_log = NULL;
int index_terminal_log = NULL;

char* key_char = "\0\0271234567890-=\x08\x09QWERTYUIOP[]\n\0ASDFGHJKL;'`\0\\ZXCVBNM,./\0*\0 \0";
int main()
{
    memory_initialize();

    VDrider_ClearScreen(0x000000);

    char* text_buffer = (char*) malloc(1024);
    terminal_log = (char*) malloc(0x2000);
    index_terminal_log = 0;

    int index_buffer = 0;
    char* user = (char*) malloc(13);
    
    memcpy(user, "Hello, World", 13);
    printf("%s This Text\n", user);
    printf(">> ");
    while( true )
    {
        uint8_t status = getkeystatus(0x64);
        if( status & 0b1 )
        {
            uint8_t scancode = getkeypress();
            if( scancode < 0x80 && scancode != 0)
            {
                if( scancode == 0x1C )
                {
                    text_buffer[index_buffer++] = 0;
                    VDriver_PutC(key_char[scancode]);
                    command(text_buffer, &index_buffer);
                    printf(">> ");
                }
                else if ( scancode == 0x0E )
                {
                    index_buffer--;
                    text_buffer[index_buffer] = 0;
                    VDriver_DecColumn();
                    VDriver_PutC(' ');
                    VDriver_DecColumn();
                }
                else {
                    text_buffer[index_buffer++] = key_char[scancode];
                    VDriver_PutC(key_char[scancode]);
                }
            }
        }
    }
    free( user );
    free( text_buffer );
    return 0x0;
}

void command(char* text_buffer, int* index_buffer) {
    if( strcmp("CLEAR", text_buffer) == 0 )
    {
        VDrider_ClearScreen(0x000000);
    }
    else if ( strncmp("PRINT", text_buffer, 5) == 0 )
    {

    }

    memset(text_buffer, 0, *index_buffer);
    *index_buffer = 0;
}