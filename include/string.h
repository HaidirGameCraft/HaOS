#ifndef STRING_HEADER
#define STRING_HEADER

#include <stdint.h>

typedef struct {
    int size;
    char** buffer;
} string_array_t;

typedef struct {
    int size;
    char* buffer;
} string_t;

string_array_t* str_array_new();
string_array_t* str_split(const char* buffer, char _delim);
void str_array_push(string_array_t* array, const char* str);
char* str_array_get(string_array_t* array, int index);
void str_array_clear(string_array_t* array);

void str_push(string_t* str, const char* buffer);
void str_clear(string_t* str);

void memset(void* buffer, uint8_t value, size_t size);
void memcpy(void* buffer, void* _src, size_t size);

size_t strlen(char* text);
char* strsubst(char* text, int start, int end);
uint8_t strcmp(const char* _cmp1, const char* _cmp2);
uint8_t strncmp(const char* _cmp1, const char* _cmp2, size_t _size);
void strreverse(char* buffer);
char tolowercase(char _c);
char touppercase(char _c);

#endif