#include <time.h>
#include <cpu/cpu.h>
#include <serial.h>

static dword __time = 0;
void time_init() {
    serial_printf("[Interrupt]: Installing Time Interrupt\n");
    interrupt_register( 32, (dword) irq_time_handle );
}

void irq_time_handle() {
    __time++;
}

dword get_time() {
    return __time;
}