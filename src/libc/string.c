#include <syscall.h>
#include <string.h>
#include <stdio.h>
#include <memory.h>

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

void strreverse(char* buffer) {
    size_t length = strlen( buffer );
    int j = length - 1;
    for(int i = 0; i < j; i++)
    {
        char tmp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = tmp;
        j--;
    }
}

char* strsubst(char* text, int start, int end) {
    if( end <= start )
        return NULL;
    
    char* b = (char*) malloc( (end - start) * sizeof( char ) );
    memcpy(b, &text[start], end - start);
    return b;
}

char tolowercase(char _c) {
    return (_c >= 65 && _c <= 90 ) ? _c + 32 : _c;
}

char touppercase(char _c) {
    return (_c >= 97 && _c <= 122 ) ? _c - 32 : _c;
}

// string_array_t* str_array_new() {
//     string_array_t* array = (string_array_t*) _malloc( sizeof( string_array_t ) );
//     memset(array, 0, sizeof( string_array_t ));
//     return array;
// }
// void str_array_push(string_array_t* array, const char* str) {
//     if( array->buffer == 0 )
//     {
//         array->buffer = (char**) _malloc( sizeof(char*) );
//     } else {
//         array->buffer = (char**) realloc(array->buffer, sizeof(char*) * (array->size + 1) );
//     }

//     array->buffer[array->size] = (char*) _malloc( strlen( str ) + 1);
//     memset((void*) array->buffer[array->size], 0, strlen(str) + 1);
//     memcpy((void*) array->buffer[array->size], str, strlen(str));
//     array->size++;
// }

// char* str_array_get(string_array_t* array, int index) {
//     return (char*) array->buffer[index];
// }

// void str_array_clear(string_array_t* array) {
//     _free( array->buffer );
//     array->size = 0;
// }

// string_array_t* str_split(const char* buffer, char _delim) {
//     string_array_t* str = str_array_new();
//     int index = 0;
//     int length = strlen( buffer );
//     while( index < length )
//     {
//         char* b = (char*) _malloc(1);
//         int _bl = 1;
//         while( buffer[index] != _delim && buffer[index] != 0 )
//         {
//             b[_bl - 1] = buffer[index];
//             _bl++;
//             b = (char*) realloc(b, _bl);
//             index++;
//         }
//         index++;
//         b[_bl - 1] = 0;
//         str_array_push(str, b);
//         _free(b);
//     }
//     return str;
// }

// void str_push(string_t* str, const char* buffer) {
//     int length = strlen( buffer );
//     if( str->buffer == 0 )
//     {
//         str->buffer = (char*) _malloc( length + 1 );
//         memcpy(str->buffer, buffer, length + 1);
//     } else {
//         str->buffer = (char*) realloc(str->buffer, strlen( str->buffer ) + 1 + length + 1);
//         memcpy((void*) &str->buffer[str->size], buffer, length + 1);
//     }
//     str->size += length;
// }
// void str_clear(string_t* str) {
//     _free(str->buffer);
//     str->size = 0;
// }