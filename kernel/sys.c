#include <system/sys.h>

#include <memory.h>
#include <string.h>
#include <elf/elf.h>
#include <syscall.h>

int execvf(const char* filePath) {
    uint32_t __fd = open(filePath, "r");
    size_t size = getsize(__fd);

    char* buffer = (char*) malloc( size );
    read(__fd, buffer, size);
    ELF32_Header header = ELF_readHeaderFile(buffer);
    ELF_readFile(&header, buffer);
    free( buffer );
    close( __fd );

    int (*_main)() = (int (*)()) header.entry;
    return _main();
}