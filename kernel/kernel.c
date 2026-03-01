
#include <header.h>
#include <driver/video_driver.h>

#include <alloc.h>
#include <page.h>
#include <serial.h>
#include <cpu/cpu.h>
#include <io.h>
#include <colorcode.h>
#include <term.h>

void kernel_main( bootstage_info_t* bootstage_info )
{
    video_driver_init( bootstage_info );
    page_init();
    video_driver_mapped();

    term_init();
    video_driver_clearScreen( R8G8B8(0, 0, 0) );
    gdt_init();
    idt_init();
    init_alloc();
    // printf("Hai %x\n", 0xAB);

    term_run();
}