#pragma once

#include <type.h>


void memory_zero( void* dest, dword size );
void memory_copy( void* dest, void* src, dword size );

dword strsize( char* text );
void strupper( char* text );
void strlower( char* text );
dword strcmp( const char* c1, const char* c2 );
dword strncmp( const char* c1, const char* c2, dword size );
void  intstr( char* buffer, int value );
void  hexstr( char* buffer, dword value );
void  revstr( char* buffer );