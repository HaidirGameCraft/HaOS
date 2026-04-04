#pragma once

#include <cpu/syscall.h>
#include <string.h>

void print( const char* text ) {
    int index = 0;
    while( text[index] != 0 )
        putc( text[index++] );
}

void printf_impl( qword* stack ) {
    const char* format = ( char* )( stack[0] );
    qword* args = ( qword* )( &stack[1] );

    int index = 0;
    char tmp_num[32];
    while( format[ index ] != 0 )
    {
        if( format[ index ] == '%' )
        {
            index = index + 1;
            if( format[index] == 'i' )
            {
                intstr( (int) *args, tmp_num );
                args++;
                print( tmp_num );
            } else if ( format[index] == 'x' )
            {
                hexstr( (qword) *args, tmp_num );
                args++;
                print( tmp_num );
            } else if ( format[index] == 's' )
            {
                print( (char*) *args );
                args++;
            } else if ( format[index] == 'c' )
            {
                putc( (char) *args );
                args++;
            } else {
                putc(format[index - 1]);
                putc(format[index]);
            }

            index = index + 1;
        } else {
            putc( format[index] );
            index = index + 1;
        }
    }
}
