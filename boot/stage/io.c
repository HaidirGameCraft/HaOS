#include "io.h"
#include "string.h"

void print( const char* text ) {
    int i = 0;
    while( text[i] != 0 )
        serial_write( COM1_PORT, text[i++] );
}

void printf( const char* format, ... ) {
    dword* ebp = (dword*)( &format + 1 );
    int idx = 0;
    static char number_buffer[20];
    while( format[idx] != 0 )
    {
        if( format[idx] == '%' )
        {
            if ( format[idx + 1] == 's' )
            {
                print( (char*) (*ebp++) );
            }
            else if ( format[idx + 1] == 'i' )
            {
                intstr( number_buffer, (*ebp++) );
                print( number_buffer );
            } else if ( format[idx + 1] == 'x' )
            {
                hexstr( number_buffer, (*ebp++) );
                print( number_buffer );
            }
            else if ( format[idx + 1] == 'c' )
            {
                serial_write( 0x3F8, (*ebp++) );
            }
            else {
                serial_write( 0x3f8, format[idx] );
                serial_write( 0x3f8, format[idx + 1] );
            }
            idx = idx + 2;
        } else {
            serial_write( 0x3f8, format[idx] );
            idx++;
        }
    }
}