#pragma once

#include <type.h>


typedef struct {
    dword magic_number;
    dword version;
    word width_char;
    word height_char;
    word count_char;
    word zeroes;
    qword offset;
} __attribute__((packed)) font_bitmap_header_t;
