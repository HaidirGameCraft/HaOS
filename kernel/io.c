#include <io.h>
#include <driver/keyboard_driver.h>
#include <term.h>
#include <string.h>

void print( const char* text )
{
    int index = 0;
    while( text[index] != 0 )
        term_putc( text[index++] );
}

void printf( const char* format, ... ) {
    dword* ebp = (dword*) &format;
    ebp++; // Skipping the first argument

    int index = 0;
    char tmp_num[20];
    while( format[ index ] != 0 )
    {
        if( format[ index ] == '%' )
        {
            index = index + 1;
            if( format[index] == 'i' )
            {
                intstr( (int) *ebp, tmp_num );
                ebp++;
                print( tmp_num );
            } else if ( format[index] == 'x' )
            {
                hexstr( (dword) *ebp, tmp_num );
                ebp++;
                print( tmp_num );
            } else if ( format[index] == 's' )
            {
                printf( (char*) *ebp );
                ebp++;
            } else if ( format[index] == 'c' )
            {
                term_putc( (char) *ebp );
                ebp++;
            } else {
                term_putc(format[index - 1]);
                term_putc(format[index]);
            }

            index = index + 1;
        } else {
            term_putc( format[index] );
            index = index + 1;
        }
    }
}

char getchar() {
    while( keyboard_is_output_serve() != 1 );
    return (char) keyboard_getchar();
}