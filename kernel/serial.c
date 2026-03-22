#include <serial.h>
#include <string.h>

void serial_print( const char* text ) {
    int i = 0;
    while( text[i] != 0 )
        serial_write( 0x3F8, text[i++] );
}
void serial_printf_implementation( qword* rsp ) {
    char* format = ( char* )( rsp[0] );
    qword* stack = ( qword* )( &rsp[1] );
    int i = 0;
    char ntmp[64];

    while( format[i] != 0 )
    {
        if( format[i] == '%' )
        {
            if( format[i + 1] == 'i' )
            {
                intstr( (int)(*stack++), ntmp );
                serial_print( ntmp );
            } else if ( format[i + 1] == 'x' )
            {
                hexstr( (qword)(*stack++), ntmp );
                serial_print( ntmp );
            } else if ( format[i + 1] == 's' )
            {
                serial_print( (char*)(*stack++) );
            } else if ( format[i + 1] == 'c' )
            {
                serial_write( 0x3F8, (char)(*stack++) );
            } else {
                serial_write( 0x3F8, format[i] );
                serial_write( 0x3F8, format[i + 1] );
            }
            i = i + 1;
        } else {
            serial_write( 0x3F8, format[i] );
        }
        i = i + 1;
    }
}