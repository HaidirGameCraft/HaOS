#include <string.h>

void memset(void* buffer, uint8_t value, size_t size) {
    for(int i = 0; i < size; i++)
    {
        ((uint8_t*) buffer)[i] = value;
    }
}

void memcpy(void* buffer, void* _src, size_t size) {
    uint32_t __s = (int)(size / sizeof( uint32_t));
    for(int i = 0; i < __s; i++)
    {
        ((uint32_t*) buffer)[i] = ((uint32_t*) _src)[i];
    }

    uint32_t _s = size % sizeof( uint32_t );
    if( _s != 0 )
    {
        for(int i = 0; i < _s; i++)
        {
            *((uint8_t*) buffer + __s * sizeof( uint32_t ) + i) = *((uint8_t*) _src + __s * sizeof( uint32_t ) + i);
        }
    }
}

size_t strlen(char* text) {
    int count = 0;
    while( text[count] != 0)
        count++;

    return count;
}
uint8_t strcmp(const char* _cmp1, const char* _cmp2) {
    size_t lcmp1 = strlen( _cmp1 );
    size_t lcmp2 = strlen( _cmp2 );

    if( lcmp1 != lcmp2 ) return 1;

    for(int i = 0; i < lcmp1; i++)
    {
        if( _cmp1[i] != _cmp2[i] )
            return 1;
    }

    return 0;
}

uint8_t strncmp(const char* _cmp1, const char* _cmp2, size_t _size) {
    for(int i = 0; i < _size; i++)
    {
        if( _cmp1[i] != _cmp2[i] )
            return 1;
    }

    return 0;
}