#include <driver/video_driver.h>
#include <font.h>
#include <string.h>

VesaInfoBlock* vesa_block;
VesaModeInfo* vesa_mode;
uint32_t* framebuffer = 0;
uint8_t bytes_per_pixel;
uint16_t width_screen = 800;
uint16_t height_screen = 600;

uint16_t max_width_char = 0;
uint16_t max_height_char = 0;

int video_lines = 0;
int video_rows = 0;

void VDriver_Initialize(VesaInfoBlock* vesa_info_block, VesaModeInfo* vesa_mode_info) {
    vesa_block = vesa_info_block;
    vesa_mode = vesa_mode_info;

    width_screen = vesa_mode->width;
    height_screen = vesa_mode->height;
    framebuffer = (uint32_t*) vesa_mode->framebuffer;
    bytes_per_pixel = vesa_mode->bytes_per_pixel / 8;

    max_width_char = width_screen / 8;
    max_height_char = height_screen / 16;
}

void VDrider_ClearScreen(COLOR color) {
    video_lines = 0;
    video_rows = 0;

    for(int i = 0; i < height_screen; i++)
    {
        for(int j = 0; j < width_screen; j++)
        {
            uint32_t index = (j + i * width_screen) * bytes_per_pixel;
            ((uint8_t*) framebuffer)[index] = color & 0xFF;
            ((uint8_t*) framebuffer)[index + 1] = color >> 8 & 0xFF;
            ((uint8_t*) framebuffer)[index + 2] = color >> 16 & 0xFF;
        }
    }
}

uint32_t color = 0x00FFFFFF;
void VDriver_DrawChar(char __c, int x, int y)
{
    uint8_t* bitmap = Font_GetBitmap(__c);
    int _y = 0;
    for(int i = 0; i < 16; i++)
    {
        int _x = 0;
        uint8_t b = bitmap[i];
        for(int j = 0; j < 8; j++)
        {
            uint32_t index = ((_x + x) + (_y + y) * width_screen) * bytes_per_pixel;
            ((uint8_t*) framebuffer)[index] = (b >> (7 - j)) & 0x1 ? (color & 0xFF) : 0;
            ((uint8_t*) framebuffer)[index + 1] = (b >> (7 - j)) & 0x1 ? (color >> 8 & 0xFF) : 0;
            ((uint8_t*) framebuffer)[index + 2] = (b >> (7 - j)) & 0x1 ? (color >> 16 & 0xFF) : 0;
            _x++;
        }
        _y++;
    }
}

void VDriver_PutC(char _c)
{
    uint32_t index = video_rows + video_lines * max_width_char;

    if( _c == '\n' )
    {
        video_rows = 0;
        video_lines++;
        index = video_rows + video_lines * max_width_char;
        video_lines = (int)(index / max_width_char);
        video_rows = index % max_width_char;

        return;
    }

    VDriver_DrawChar(_c, video_rows * 8, video_lines * 16);
    video_rows++;

    index = video_rows + video_lines * max_width_char;
    video_lines = (int)(index / max_width_char);
    video_rows = index % max_width_char;
}