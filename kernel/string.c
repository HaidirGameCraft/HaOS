#include <string.h>
#include <serial.h>

// void memzero( void* buffer, size_t size ) {
//     for( qword i = 0; i < size; i++ )
//     {
//     }
// }
// void memcopy( void* dest, void* src, size_t size ) {
//     byte* __dest = ( byte* ) dest;
//     byte* __src = ( byte* ) src;

    
//     for( qword i = 0; i < size; i++ ) {
//         __asm__ volatile("movq %0, %%rsi" :: "r"(__src) );
//         __asm__ volatile("movq %0, %%rdi" :: "r"(__dest) );
//         __asm__ volatile("movb (%rsi), %al");
//         __asm__ volatile("movb %al, (%rdi)");
//         __asm__ volatile("incq %rsi\n\t incq %rdi");
//         __asm__ volatile("mov %%rsi, %0" : "=r"(__src) );
//         __asm__ volatile("mov %%rdi, %0" : "=r"(__dest) );
//     }
// }

size_t strsize( char* buffer ) {
    size_t index = 0;
    while( buffer[index] != 0 ) index++;
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

int     strcmp( const char* c1, const char* c2 ) {
    size_t s1 = strsize( c1 );
    size_t s2 = strsize( c2 );

    if( s1 != s2 )  return 1;
    return strncmp( c1, c2, s1 );
}

int     strncmp( const char* c1, const char* c2, size_t size ){
    for( int i = 0; i < size; i++ )
        if( c1[i] != c2[i] ) return 1;
    return 0;
}
void    strrev( char* buffer ) {
    int i = 0, j = strsize( buffer ) - 1;
    while( i < j )
    {
        char tmp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = tmp;
        i = i + 1;
        j = j - 1;
    }
}
void    intstr( int value, char* buffer ) {
    int index = 0;
    byte sign = ( value >= 0 ) ? 1 : 0;
    if( sign == 0 ) value = value * -1;
    if( value != 0 )
    {
        while( value > 0 )
        {
            char _c = ( value % 10 ) + '0';
            buffer[index++] = _c;
            value = value / 10;
        }
    } else {
        buffer[index++] = '0';
    }
    
    if( sign == 0 ) buffer[index++] = '-';
    buffer[index] = 0;
    strrev( buffer );
}
void    hexstr( qword value, char* buffer ) {
    int index = 0;
    static const char* hexw = "0123456789ABCDEF";
    for( int i = 0; i < 16; i++ )
    {

        if( value != 0 )
        {
            char _c = hexw[ (qword)(value & 0x0F) ];
            value = value >> 4;
            buffer[index++] = _c;
        } else {
            // buffer[index++] = '0';
        }
    }
    if( index == 0 )
    {
        buffer[index++] = '0';
        buffer[index++] = '0';
    }
    buffer[index] = 0;
    strrev( buffer );
}

int     strint( char* text ) {
    int len = strsize( text );
    int val = 0;
    int index = 0;
    byte sign = 0;
    if( text[index] == '-' )
    {
        sign = 1;
        index++;
    }

    while( index < len )
    {
        char v = text[index];
        val = val * 10;
        val = val + (v - '0');
        index++;
    }

    if( sign == 1 ) val = val * -1;
    return val;
}

dword   strhex( char* text ) {
    // TODO
}
