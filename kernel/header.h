#pragma once

#include <type.h>

typedef struct {
    char identifier[11];
    qword address_start;
    qword address_end;
    qword entry_start;

    qword font_data;

    dword width;
    dword height;
    byte  bytes_per_pixel;
    qword framebuffer;
    dword size_of_framebuffer;
} __attribute__((packed)) kernel_header_info_t;

extern kernel_header_info_t header;

typedef struct {
    qword identifier;
    qword flags;

    // Paging Infermation
    qword pml4;

    // For Video Mode Information
    word width;
    word height;
    word bytes_per_pixel;
    qword framebuffer;
} __attribute__((packed)) bootstage_info_t;