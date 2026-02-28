#include <string.h>

void memory_zero( void* dest, dword size ) {
    for( int i = 0; i < size; i++ )
        ((byte*) dest)[i] = 0;
}
void memory_copy( void* dest, void* src, dword size ) {
    for( int i = 0; i < size; i++ )
        ((byte*) dest)[i] = ((byte*) src)[i];
}

dword strsize( char* text ) {
    int index = 0;
    while( text[index] != 0 )
        index++;
    return index;
}
void strupper( char* text ) {
    dword size = strsize( text );
    for( dword i = 0; i < size; i++ )
    {
        if( text[i] >= 'a' && text[i] <= 'z' )
            text[i] = text[i] - 'a' + 'A';
    }
}
void strlower( char* text ) {
    dword size = strsize( text );
    for( dword i = 0; i < size; i++ )
    {
        if( text[i] >= 'A' && text[i] <= 'Z' )
            text[i] = text[i] - 'A' + 'a';
    }
}

dword strcmp( const char* c1, const char* c2 ) {
    dword s1 = strsize( c1 ), s2 = strsize( c2 );
    if( s1 != s2 )  return 1;

    return strncmp( c1, c2, s1 );
}
dword strncmp( const char* c1, const char* c2, dword size ) {
    for( int i = 0; i < size; i++ )
        if( c1[i] != c2[i] )    return 1;
    return 0;
}

void  intstr( char* buffer, int value ) {
    byte sign = value > 0 ? 1 : 0;
    int index = 0;
    int temp = value;
    if( temp < 0 )
        temp = -temp;
    
    while( temp > 0 )
    {
        char _c = (temp % 10) + '0';
        temp = temp / 10;
        buffer[index] = _c;
        index++;
    }

    if( !sign ) buffer[index++] = '-';
    buffer[index] = 0;
    revstr( buffer );
}

void  hexstr( char* buffer, dword value ) {
    const char* hexw = "0123456789ABCDEF";
    int index = 0;
    int len = 8;
    while( len > 0 )
    {
        if( value != 0 ) {
            int _i = value & 0xF;
            char _c = hexw[ _i ];
            buffer[index++] = _c;
            value = value >> 4;
        } else {
            buffer[index++] = '0';
        }
        len = len - 1;
    }

    buffer[index] = 0;
    revstr( buffer );
}

void  revstr( char* buffer ) {
    int i = 0;
    int j = strsize( buffer ) - 1;
    while( i < j )
    {
        // Swap the variable
        char t = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = t;
        i++;
        j--;
    }
}