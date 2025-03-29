#include <driver/vga_driver.h>

#define WIDTH_CHAR_SCREEN 80
#define HEIGHT_CHAR_SCREEN 25

struct vga_char_struct {
    uint8_t character;
    uint8_t color;
};

int lines = 0;
int rows = 0;
struct vga_char_struct* video_memory = (struct vga_char_struct*) 0xb8000;

void vga_clearscreen(uint32_t color) {
    lines = 0;
    rows = 0;

    for(int i = 0; i < WIDTH_CHAR_SCREEN * HEIGHT_CHAR_SCREEN; i++)
    {
        video_memory[i].color = 0x0F;
        video_memory[i].character = ' ';
    }
}

void vga_putc(uint8_t c) {
    if( c == '\n' )
    {
        lines++;
        rows = 0;
        return;
    }

    uint32_t index = rows + lines * WIDTH_CHAR_SCREEN;
    video_memory[index].color = 0x0F;
    video_memory[index].character = c;
    rows++;

    index = rows + lines * WIDTH_CHAR_SCREEN;
    lines = (int)(index / WIDTH_CHAR_SCREEN);
    rows = index % WIDTH_CHAR_SCREEN;
}