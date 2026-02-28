#pragma once

#include <type.h>

void memzero( void* buffer, size_t size );
void memcopy( void* dest, void* src, size_t size );

size_t strsize( char* buffer );
void strupper( char* text );
void strlower( char* text );
int     strcmp( const char* c1, const char* c2 );
int     strncmp( const char* c1, const char* c2, size_t size );
void    strrev( char* buffer );
void    intstr( int value, char* buffer );
void    hexstr( dword value, char* buffer );
