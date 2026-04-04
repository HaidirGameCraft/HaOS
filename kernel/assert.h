#pragma once
#include <io.h>


#define assert( expr ) ( !(expr) ) ? __assert_function( #expr, __FILE__, __LINE__ ) : 0;


static void __assert_function( const char* expr, const char* file, int line ) {
    printf("[Assertation Abort Kernel]: %s at %s:%i", expr, file, line );
    while( 1 )
        __asm__ volatile("hlt");

}
