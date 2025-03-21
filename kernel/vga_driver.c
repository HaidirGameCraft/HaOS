#include <driver/vga_driver.h>
#include <string.h>

#define WIDTH_CHAR_SCREEN 80
#define HEIGHT_CHAR_SCREEN 25

struct vga_char_struct {
    uint8_t character;
    uint8_t color;
};

int lines = 0;
int rows = 0;
int prev_lines = 0;
int prev_rows = 0;
struct vga_char_struct* video_memory = (struct vga_char_struct*) 0xb8000;

void vga_scroll_up();

void vga_clearscreen(uint32_t color) {
    lines = 0;
    rows = 0;
    prev_lines = 0;
    prev_rows = 0;

    for(int i = 0; i < WIDTH_CHAR_SCREEN * HEIGHT_CHAR_SCREEN; i++)
    {
        video_memory[i].color = 0x0F;
        video_memory[i].character = ' ';
    }
}

void vga_putc(uint8_t c) {
    uint32_t index = rows + lines * WIDTH_CHAR_SCREEN;
    if( c == '\n' )
    {
        video_memory[index].color = 0x0F00;
        lines++;
        rows = 0;

        if( lines < HEIGHT_CHAR_SCREEN )
        {
            index = rows + lines * WIDTH_CHAR_SCREEN;
            video_memory[index].color = ~video_memory[index].color;
        }

        prev_lines = lines;
        prev_rows = rows;
        vga_scroll_up();
        return;
    }

    prev_lines = lines;
    prev_rows = rows;

    
    video_memory[index].color = 0x0F;
    video_memory[index].character = c;
    rows++;

    index = rows + lines * WIDTH_CHAR_SCREEN;
    video_memory[index].color = ~video_memory[index].color;
    lines = (int)(index / WIDTH_CHAR_SCREEN);
    rows = index % WIDTH_CHAR_SCREEN;

    vga_scroll_up();
}

void vga_scroll_up() {
    if( lines < HEIGHT_CHAR_SCREEN )
        return;

    for(int i = 1; i < HEIGHT_CHAR_SCREEN; i++)
    {
        memcpy((void*) 0xb8000 + (i - 1) * WIDTH_CHAR_SCREEN * 2,(void*) 0xb8000 + i * WIDTH_CHAR_SCREEN * 2, WIDTH_CHAR_SCREEN * 2);
    }

    memset((void*) 0xb8000 + (HEIGHT_CHAR_SCREEN - 1) * WIDTH_CHAR_SCREEN * 2, 0, WIDTH_CHAR_SCREEN * 2);
    lines--;
    uint32_t index = rows + lines * WIDTH_CHAR_SCREEN;
    video_memory[index].color = ~video_memory[index].color;
}