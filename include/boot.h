#ifndef BOOT_HEADER
#define BOOT_HEADER

#include <stdint.h>
#include <driver/video_driver.h>

typedef struct {
    addr_t font_address;
    VesaInfoBlock* vesa_info_block;
    VesaModeInfo* vesa_mode_info;
} C_PACKED BootInfo_t;

#endif