#pragma once

/*  
 *
 * */

#include <type.h>
#include <cpu/cpu.h>

#define TASK_FLAGS_RUNNING_BIT     ( 1 << 0 )

struct task_info_t {
    qword   id;                  // id of each Task
    addr_t  stack_provider;  // original stack from page_alloc4K
    addr_t  stack;   // stack of each task
    qword   flags;
    qword   signal_code;
    struct task_info_t* next; // next task (NULL - returning to main)
} __attribute__((packed));

void task_init();
extern void task_create( addr_t address, int argc, ... );

void task_print();
void task_exit( qword signal );
dword task_getid();

// use on IRQ0 only
extern void task_remove( );

byte is_ctask_running();

extern void switch_task( cpu_register_t* cpu );
extern void switch_next_task_imme();