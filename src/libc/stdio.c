#include <stdio.h>
#include <syscall.h>
#include <driver/video_driver.h>

void printf(const char* format, ...) {
    uint32_t* ptr = (uint32_t*) &format;
    int idptr = 1;

    // printh(ptr[1], 4);
    
    int index = 0;
    while( format[index] != 0 )
    {
        
        if( format[index] == '%' )
        {
            if( format[index + 1] == 's' )
            {
                index += 2;
                addr_t s = ptr[idptr++];
                if( s != NULL )
                    printf((char*) s);
            }
            else if( format[index + 1] == 'x' ) {
                index += 2;
                printh((uint32_t) ptr[idptr++], 4);
            }
            else if( format[index + 1] == 'i' ) {
                index += 2;
                // printi((int) ptr[idptr++]);
            }
            else if ( format[index + 1] == 'l')
            {
                index += 2;
                if( format[index] == 'x')
                {
                    index++;
                    printh((uint32_t) ptr[idptr++], 8);
                }
                else if ( format[index] == 'i' )
                {

                }
            }
        }
        else {
            VDriver_PutC( format[index] );
            index++;
        }
        
    }
}

void printh(uint32_t value, size_t size_byte) {
    uint32_t val = value;
    char* words_hex = "0123456789ABCDEF";
    for(int i = 0; i < size_byte; i++)
    {
        for(int j = 0; j < 2; j++)
        {
            char c = words_hex[((value >> ((size_byte - 1) - i) * 8)) >> (1 - j) * 4 & 0x0F];
            VDriver_PutC(c);
        }
    }
}

// void printi(int value) {
//     int tmp = value;

//     if( value < 0)
//         tmp = -tmp;

//     char* b = (char*) _malloc(1);
//     int length = 1;

//     while( tmp > 0 )
//     {
//         char f = (tmp % 10) + '0';
//         b[length - 1] = f;
//         length++;
//         b = realloc(b, length);
//         tmp /= 10;
//     }
//     b[length - 1] = 0;

//     strreverse(b);
//     printf(b);
//     _free(b);
// }