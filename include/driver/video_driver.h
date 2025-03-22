#ifndef VIDEO_DRIVER_HEADER
#define VIDEO_DRIVER_HEADER

#include <stdint.h>

typedef struct {
    char signature[4];
    uint16_t version;
    addr_t OemStringPtr;
    uint32_t Capabilities;
    addr_t videoModePtr;
    uint16_t total_memory;
    uint8_t reserved[492];
} C_PACKED VesaInfoBlock;

typedef struct {
    uint16_t attributes;
    uint8_t first_window;
    uint8_t second_window;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t first_segment;
    uint16_t second_segment;
    addr_t window_function_ptr;
    uint16_t pitch;

    uint16_t width;
    uint16_t height;
    uint8_t width_char;
    uint8_t height_char;
    uint8_t planes;
    uint8_t bytes_per_pixel;
    uint8_t banks;
    uint8_t memory_model;
    uint8_t bank_size;
    uint8_t image_pages;
    uint8_t reserved_0;

    uint8_t red_mask;
    uint8_t red_position;
    uint8_t green_mask;
    uint8_t green_position;
    uint8_t blue_mask;
    uint8_t blue_position;
    uint8_t reserved_mask;
    uint8_t reserved_position;
    uint8_t direct_color;

    addr_t framebuffer;
    uint32_t offscreen_offset;
    uint16_t offscreen_segment;
    uint8_t reserved_2[206];
} C_PACKED VesaModeInfo;

typedef uint32_t COLOR;

void VDriver_Initialize(VesaInfoBlock* vesa_info_block, VesaModeInfo* vesa_mode_info);
void VDriver_Init(uint16_t width, uint16_t height, uint8_t bpp, uint32_t addr);
void VDrider_ClearScreen(COLOR color);
void VDriver_PutC(char _c);
void VDriver_PutPixel(int x, int y, COLOR _color);
void VDriver_CopyPixel(uint32_t from, uint32_t to);
void VDriver_IncColumn();
void VDriver_DecColumn();
void VDriver_IncLine();
void VDriver_DecLine();
uint8_t* VDriver_GetAddress();
uint32_t VDriver_GetPixel(int x, int y);
uint16_t VDriver_GetWidth();
uint16_t VDriver_GetHeight();

#endif