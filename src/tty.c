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
    string_array_t* argv = str_split(text_buffer, ' ');
    char* cmd = str_array_get(argv, 0);

    if( cmd == NULL )
    {
        printf("Not a Command...\n");
        str_array_clear( argv );
        free( argv );
        
        memset(text_buffer, 0, *index_buffer);
        *index_buffer = 0;
        return;
    }

    if( strcmp("CLEAR", cmd) == 0 )
    {
        VDrider_ClearScreen(0x000000);
    }
    else if ( strcmp("PRINT", cmd) == 0 )
    {
        for(int i = 1; i < argv->size; i++)
        {
            printf("%s ", str_array_get(argv, i));
        }
        printf("\n");
    }
    else if (strcmp("MKDIR", cmd) == 0 )
    {
        if( str_array_get(argv, 1) == NULL )
        {
            printf("Error> No File Path\n");
        }
        else {
            uint32_t result = mkdir(str_array_get(argv, 1));
            if( result == ENTRY_NO_PARENT )
            {
                printf("Error> Parent Path is Not Exists\n");
            }
            else if ( result == ENTRY_PARENT_NOT_DIRECTORY )
            {
                printf("Error> Parent Path is Not Directory\n");
            }
        }
    }
    else if ( strcmp("LS", cmd) == 0 )
    {
        uint32_t fd = opendir("");
        FILE* __output = (FILE*) malloc( sizeof( FILE ) );
        while( readdir(fd, __output ) == 0 )
        {
            printf("%s \n", __output->filename);
            
            _free( __output->filename );
            free( __output );
        }
        closedir( fd );
    }

    str_array_clear( argv );
    free( argv );

    memset(text_buffer, 0, *index_buffer);
    *index_buffer = 0;
}