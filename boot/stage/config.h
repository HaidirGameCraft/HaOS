#pragma once

#include <type.h>

typedef struct {
    char identifier[11];
    dword address_start;
    dword address_end;
    dword entry_start;
} __attribute__((packed)) kernel_file_header_t;

typedef struct {
    byte signature[4];
    word version;
    dword string_ptr;
    byte capablities[4];
    dword videoMode_ptr;
    word total_memory;
    byte reserved[492];
} VesaBlock_Info_t;

typedef struct {
    word attributes;
    byte firstWindow;
    byte secondWindow;
    word granularity;
    word windowSize;
    word firstSegment;
    word secondSegment;
    dword window_ptr;
    word pitch;
    word width;
    word height;
    byte widthChar;
    byte heightChar;
    byte planes;
    byte bpp;
    byte banks;
    byte memoryModel;
    byte bankSize;
    byte imagePages;
    byte reserved;

    byte redMask;
    byte redPosition;
    byte greenMask;
    byte greenPosition;
    byte blueMask;
    byte bluePosition;
    byte alphaMask;
    byte alphaPosition;
    byte directColor;

    dword framebuffer;
    dword offScreen_memOff;
    word offScreen_memSize;
} __attribute__((packed)) VesaMode_Info_t;

#define BOOTSTAGE_FLAGS_GRAPHICS_MODE_BIT   1

typedef struct {
    dword identifier;
    dword flags;
    // Video Info
    word width;
    word height;
    word bytes_per_pixel;
    dword framebuffer;
} __attribute__((packed)) bootstage_info_t;

extern VesaBlock_Info_t vesaInfo;
extern VesaMode_Info_t vesaModeInfo;
extern void __BOOTSTAGE_START();
extern void __BOOTSTAGE_END();