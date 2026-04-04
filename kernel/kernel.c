
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
#include <process/users.h>
#include <process/task.h>

void test_task() {
    printf("Hello, World from Task\n");
    task_exit( 0xDEAD1234 );
}

void kernel_main( )
{
    
    video_driver_init( );
    page_init();
    video_driver_mapped();
    
    video_driver_clearScreen( R8G8B8(100, 0, 0) );
    
    term_init();
    
    gdt_init();
    task_init();
    idt_init();
    init_alloc();
    fat_init();

    
    // Remove it if you want
    video_driver_clearScreen( R8G8B8(0, 0, 0) );
    printf("HaOS Created By Haidir\n\n\n");

    task_create( (qword) test_task, 0 );

    // Test . Entering Sys Enter to ring 3
    //turn_to_userspace( (qword) test_ring3 );

    // Testing

    term_run();
    return;
}
