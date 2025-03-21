#ifndef STDIO_HEADER
#define STDIO_HEADER

#include <stdint.h>

#define STDIN   0
#define STDOUT  1

void printf(const char* format, ...);
void printh(uint32_t value, size_t size_byte);
void printi(int value);

#endif