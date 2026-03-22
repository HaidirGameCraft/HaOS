#include "io.h"
#include "string.h"
#include "args.h"

void print( const char* text ) {
    int i = 0;
    while( text[i] != 0 )
        x64_serial_write( COM1_PORT, text[i++] );
}

void printf_implementation( qword* rsp ) {
    char*  format = (char*)( rsp[0] );
    qword* stack = (qword*)( &rsp[1] );

    int idx = 0;
    static char number_buffer[32];
    while( format[idx] != 0 )
    {
        if( format[idx] == '%' )
        {
            if ( format[idx + 1] == 's' )
            {
                print( (char*) *stack++ );
            }
            else if ( format[idx + 1] == 'i' )
            {
                intstr( number_buffer, *stack++ );
                print( number_buffer );
            } else if ( format[idx + 1] == 'x' )
            {
                hexstr( number_buffer, *stack++ );
                print( number_buffer );
            }
            else if ( format[idx + 1] == 'c' )
            {
                x64_serial_write( 0x3F8, (char) *stack++ );
            }
            else {
                x64_serial_write( 0x3f8, format[idx] );
                x64_serial_write( 0x3f8, format[idx + 1] );
            }
            idx = idx + 2;
        } else {
            x64_serial_write( 0x3f8, format[idx] );
            idx++;
        }
    }
}