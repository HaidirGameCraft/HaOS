#include <io.h>
#include <driver/keyboard_driver.h>

void print( const char* text )
{
    // TODO
}

void printf( const char* format, ... ) {
    // TODO
}

char getchar() {
    while( keyboard_is_output_serve() != 1 );
    return (char) keyboard_getchar();
}