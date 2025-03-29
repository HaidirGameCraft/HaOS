#ifndef __FS__
#define __FS__

#include <stdint.h>

typedef struct {
    char name[256];
    int start_position;
    long position;
    size_t size;
} file_t;

typedef struct {
    char name[256];
    addr_t parent_dir;
    uint32_t cluster;
    uint32_t index;
    uint32_t reserved;
} dir_t;

DWORD fopen(const char* filename, const char* flags);
void fclose(DWORD __fd);
void fseek(DWORD __fd, long position);
long ftell(DWORD __fd);
int fread(DWORD __fd, char* __buffer, size_t __size);
int fwrite(DWORD __fd, char* __buffer, size_t __size);


#endif