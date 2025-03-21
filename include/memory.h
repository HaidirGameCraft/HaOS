#ifndef MEMORY_HEADER
#define MEMORY_HEADER

#define VIRTUAL_HEAP_MEMORY_START 0x02000000
#define VIRTUAL_HEAP_MEMORY_END     0x03000000

#include <stdint.h>

#define CREATE(T) (T*) malloc(sizeof(T))
#define DELETE(T) free(T);

void memory_initialize();
uint32_t heap_size();
void* malloc(size_t size);
void* calloc(void* ptr);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

#endif