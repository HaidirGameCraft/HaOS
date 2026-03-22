#pragma once

#include <type.h>

typedef struct {
    qword identifier;
    qword flags;

    // For Video Mode Information
    word width;
    word height;
    word bytes_per_pixel;
    dword framebuffer;
} __attribute__((packed)) bootstage_info_t;