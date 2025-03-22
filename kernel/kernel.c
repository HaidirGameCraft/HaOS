#include "kernel.h"

#include <fs/fat.h>
#include <font.h>
#include <driver/video_driver.h>
#include <driver/keyboard_driver.h>
#include <driver/disk_driver.h>
#include <timer.h>
#include <cpu/gdt.h>
#include <elf/elf.h>
#include <page.h>
#include <cpu/isr.h>
#include <string.h>
#include <memory.h>
#include <syscall.h>
#include <system/sys.h>

#include <stdio.h>

void key_callback(char sc, uint8_t key, uint8_t status);

void kernel_main() {

    BootInfo_t* boot_info = (BootInfo_t*) 0x00020000;
    memory_initialize();
    fat_initialize();

    VDriver_Initialize(boot_info->vesa_info_block, boot_info->vesa_mode_info);
    VDrider_ClearScreen(0x00000000);
    void* font_buffer = (void*) malloc(9 * 512);
    Font_InitAddress("bitfont.bin", (uint32_t) font_buffer);

    extern void _kernel_start();
    extern void _kernel_end();
    printf("Kernel Start: "); printh((uint32_t) _kernel_start, 4); printf("\n");
    printf("Kernel End: "); printh((uint32_t) _kernel_end, 4); printf("\n");
    printf("Kernel Size: "); printh((uint32_t)( _kernel_end - _kernel_start), 4); printf("\n");
    Page_Initialize();
    
    gdt_initialize();
    idt_initialize();

    __asm__ volatile("cli");
    PIT_Init(1024);
    Keyboard_Initialize();
    __asm__ volatile("sti");

    Keyboard_PushEvent( key_callback );

    const char* user = "Haidir";
    uint32_t j = 0x11223344;
    
    printf("Hello, %s World Kernel! %i\n", user, j);
    // const char* text = "welcome/to/world";
    // printf("Text: %s\n", text);
    // string_array_t* str = str_split(text, '/');
    // string_t sjoin;
    // for(int i = 0; i < str->size; i++)
    // {
    //     char* t = str_array_get(str, i);
    //     str_push(&sjoin, t);
    //     printf("%i: %s\n", i, t);
    // }

    // printf("Final Join: %s\n", sjoin.buffer);
    // str_clear(&sjoin);
    // str_array_clear( str );
    // free(str);

    make_dir("welcomeToWorld");
    make_entry("welcome/world.txt", FAT_ARCHIVE, user, strlen( user ));
    //printf("DEBUG\n");

    DIR* dir = open_dir("");
    FILE target;
    while( read_next_dir(dir, &target) == 0 )
    {
        printf("%s\n", target.filename);
    }
    
    printf("Heap Size: %x\n", heap_size());
    

    int exec = execvf("tty.elf");
    // close(fd);

    return;
}

void key_callback(char sc, uint8_t key, uint8_t status) {
    if( status == KEYBOARD_KEY_PRESS )
    {
        //VDriver_PutC( sc );
    }
}

void _kernel()
{
    printf("Hello, _Kernel\n");
    uint16_t cs, ds;
    __asm__ volatile("mov %%cs, %0" : "=r"(cs));
    __asm__ volatile("mov %%ds, %0" : "=r"(ds));
    printf("\n");
    printh(cs, 2);
    printf("\n");
    printh(ds, 2);
    printf("\n");
}