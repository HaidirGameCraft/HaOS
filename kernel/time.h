#pragma once

#include <type.h>
#include <cpu/cpu.h>

void time_init();
void irq_time_handle( cpu_register_t* cpu );

dword get_time();
