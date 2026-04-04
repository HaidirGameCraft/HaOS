#include <time.h>
#include <cpu/cpu.h>
#include <serial.h>
#include <process/task.h>

static dword __time = 0;
void time_init() {
    serial_printf("[Interrupt]: Installing Time Interrupt\n");
    interrupt_register( 32, (qword) irq_time_handle );
}

void irq_time_handle( cpu_register_t* cpu ) {
    __time++;
    if( __time % 1000 ) {
        if( !is_ctask_running() )
            task_remove( );
        //serial_printf("Switch Task...\n");
        //task_print(); 
        switch_task( cpu );
    }
}

dword get_time() {
    return __time;
}
