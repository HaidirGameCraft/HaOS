#include <driver/video_driver.h>
#include <syscall.h>

#include <font.h>
#include <string.h>

VesaInfoBlock* vesa_block;
VesaModeInfo* vesa_mode;
uint8_t* framebuffer = 0;
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
    framebuffer = (uint8_t*) vesa_mode->framebuffer;
    bytes_per_pixel = vesa_mode->bytes_per_pixel / 8;

    max_width_char = width_screen / 8;
    max_height_char = height_screen / 16;
}

void VDriver_Init(uint16_t width, uint16_t height, uint8_t bpp, uint32_t addr) {
    width_screen = width;
    height_screen = height;
    framebuffer = (uint8_t*) addr;
    bytes_per_pixel = bpp / 8;

    max_width_char = width_screen / 8;
    max_height_char = height_screen / 16;
}

void VDrider_ClearScreen(COLOR color) {
    width_screen = video_getwidthscreen();
    height_screen = video_getheightscreen();
    max_width_char = width_screen / 8;
    max_height_char = height_screen / 16;
    bytes_per_pixel = 3;

    video_lines = 0;
    video_rows = 0;

    for(int i = 0; i < height_screen; i++)
    {
        for(int j = 0; j < width_screen; j++)
        {
            video_putpixel(j, i, color);
        }
    }
}

uint32_t color = 0x00FFFFFF;
void VDriver_DrawChar(char __c, int x, int y)
{
    uint8_t* bitmap = (uint8_t*) Font_GetBitmap(__c);
    int _y = 0;
    for(int i = 0; i < 16; i++)
    {
        int _x = 0;
        uint8_t b = bitmap[i];
        for(int j = 0; j < 8; j++)
        {
            video_putpixel(_x + x, _y + y, (b >> (7 - j)) & 0x1 ? color : 0);
            _x++;
        }
        _y++;
    }
}

void VDriver_ScrollUp()
{
    if( video_lines < max_height_char - 1 )
        return;

    for(int i = 1; i < max_height_char; i++)
    {
        video_copypixel((i - 1) * width_screen * 16, i * width_screen * 16);
    }

    memset(&((uint8_t*) video_getframebuffer())[(max_height_char - 1) * width_screen * 16 * bytes_per_pixel], 0, width_screen * 16 * bytes_per_pixel);
    video_lines--;
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
        VDriver_ScrollUp();

        return;
    }

    VDriver_DrawChar(_c, video_rows * 8, video_lines * 16);
    video_rows++;

    index = video_rows + video_lines * max_width_char;
    video_lines = (int)(index / max_width_char);
    video_rows = index % max_width_char;

    VDriver_ScrollUp();
}

void VDriver_IncColumn() {
    video_rows++;
    int index = video_rows + video_lines * max_width_char;
    video_lines = (int)(index / max_width_char);
    video_rows = index % max_width_char;
}
void VDriver_DecColumn() {
    video_rows--;
    int index = video_rows + video_lines * max_width_char;
    video_lines = (int)(index / max_width_char);
    video_rows = index % max_width_char;
}
void VDriver_IncLine() {
    video_lines++;
    int index = video_rows + video_lines * max_width_char;
    video_lines = (int)(index / max_width_char);
    video_rows = index % max_width_char;
}
void VDriver_DecLine() {
    video_lines--;
    int index = video_rows + video_lines * max_width_char;
    video_lines = (int)(index / max_width_char);
    video_rows = index % max_width_char;
}

uint16_t VDriver_GetWidth() {
    return width_screen;
}
uint16_t VDriver_GetHeight() {
    return height_screen;
}