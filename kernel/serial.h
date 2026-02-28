#pragma once

#include <type.h>

extern char serial_read( word port );
extern void serial_write( word port, char c );
extern void serial_connect( word port );
extern void serial_disconnect( word port );
void serial_print( const char* text );
void serial_printf( const char* format, ... );