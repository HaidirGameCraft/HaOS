
#include <header.h>
#include <driver/video_driver.h>

#include <alloc.h>
#include <page.h>
#include <serial.h>
#include <cpu/cpu.h>
#include <io.h>
#include <colorcode.h>
#include <term.h>
#include <fs/fat.h>
#include <elf.h>

void kernel_main( bootstage_info_t* bootstage_info )
{
    
    video_driver_init( bootstage_info );
    page_init();
    video_driver_mapped();
    
    video_driver_clearScreen( R8G8B8(0, 0, 0) );
    
    term_init();
    
    gdt_init();
    idt_init();
    init_alloc();
    serial_printf("Hello, World\n");
    
    
    fat_init();

    // Trying ELF
    elf64_load("main.elf");
    
    term_run();
    return;
}
