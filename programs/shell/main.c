#include <cpu/syscall.h>
#include <io.h>

extern void putc( char c );

int main( void ) {
    printf("Hello, World\n");
    return 0xDEAD1234;
}
