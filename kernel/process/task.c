#include "task.h"
#include "cpu/cpu.h"
#include "serial.h"
#include "string.h"
#include <alloc.h>
#include <config.h>
#include <io.h>
#include <assert.h>
#include <page.h>

struct task_info_t kernel_task;
struct task_info_t* task_current = NULL;
struct task_info_t* task_next = NULL;
static dword __id_task = 1;

extern  qword* task_stack;  // Stack for Task

void task_init() {
    kernel_task.stack = (qword*) KERNEL_STACK_BOTTOM;
    kernel_task.stack_provider = (qword) KERNEL_STACK_BOTTOM;
    kernel_task.next = NULL;
    kernel_task.flags |= TASK_FLAGS_RUNNING_BIT;
    kernel_task.id = 0;

    task_current = &kernel_task;
    task_next = &kernel_task;
}

dword task_getid() {
    return task_current->id;
}

void task_create_impl( qword stack ) {
    addr_t address = ((qword*) stack)[0];
    dword __argc = ((qword*) stack)[1];
    qword* __args = &((qword*) stack)[2];


    struct task_info_t* new_task = ( struct task_info_t* ) new_alloc( sizeof( struct task_info_t ) );
    serial_printf("New Task at %x\n", (qword) new_task );
    memzero(new_task, sizeof( struct task_info_t ) );
    new_task->id = __id_task;
    new_task->stack_provider = ( qword ) page_alloc4K( PAGE_PRESENT | PAGE_READWRITE );
    new_task->stack = new_task->stack_provider + 0x1000;
    new_task->flags |= TASK_FLAGS_RUNNING_BIT;
    new_task->signal_code = 0;
    new_task->next = NULL;

    __id_task = __id_task + 1;  // increament by One

    qword old_rsp = 0;
    qword  old_cs = 0;
    qword  old_ss = 0;

    memzero( (void*)((qword) new_task->stack - 0x1000), 0x1000 );

    __asm__ volatile("mov   %%cs, %0" : "=r"( old_cs ) );
    __asm__ volatile("mov   %%ss, %0" : "=r"( old_ss ) );
    __asm__ volatile("movq  %%rsp, %0" : "=r"( old_rsp ) );
    __asm__ volatile("movq  %0, %%rsp" :: "r"( new_task->stack ) );

    __asm__ volatile("push  %0" :: "r"( old_ss ) );            // old_ss
    __asm__ volatile("push  %rsp" );
    __asm__ volatile("push  $0x202");    // flags
    __asm__ volatile("push  %0" :: "r"( old_cs ) );           // cs
    __asm__ volatile("pushq %0" :: "r"( address ));     // rip

    __asm__ volatile("subq $0x10, %rsp");
    __asm__ volatile("pushq $0");                                   // r15 
    __asm__ volatile("pushq $0");                                   // r14
    __asm__ volatile("pushq $0");                                   // r13
    __asm__ volatile("pushq $0");                                   // r12
    __asm__ volatile("pushq $0");                                   // r11
    __asm__ volatile("pushq $0");                                   // r10
    __asm__ volatile("pushq %0" :: "r"( (qword) __args[5] ) );      // r9: Sixth Argument
    __asm__ volatile("pushq %0" :: "r"( (qword) __args[4] ) );      // r8: Fifth Argument

    __asm__ volatile("pushq %0" :: "r"( (qword) __args[1] ) );      // rsi: Second Argument
    __asm__ volatile("pushq %0" :: "r"( (qword) __args[0] ) );      // rdi: First Argument
    __asm__ volatile("pushq %0" :: "r"(new_task->stack) );          // rbp
    __asm__ volatile("pushq $0");                                   // rsp ( always 0 )
    __asm__ volatile("pushq $0");                                   // rbx
    __asm__ volatile("pushq %0" :: "r"( (qword) __args[3] ) );      // rdx: Fourth Argument
    __asm__ volatile("pushq %0" :: "r"( (qword) __args[2] ) );      // rcx: Third Argument
    __asm__ volatile("pushq $0");    // rax
                                    
    __asm__ volatile("subq $0x20, %rsp");   // skip cr0, cr2, cr3 and cr4

    // __asm__ volatile("movq %rsp, %rax");
    // for( qword i = 0; i < 26; i++ )
    // {
    //     qword val = 0;
    //     qword addr = 0;
    //     __asm__ volatile("mov %0, %%rax" :: "r"(i));
    //     __asm__ volatile("movq $8, %rcx");
    //     __asm__ volatile("mul %rcx");
    //     __asm__ volatile("movq %rax, %rbx");
    //     __asm__ volatile("movq %rsp, %rax");
    //     __asm__ volatile("addq %rbx, %rax");
    //     __asm__ volatile("movq %%rax, %0" : "=r"( addr ) );
    //     __asm__ volatile("movq (%%rax), %0" : "=r"( val ) );
    //     __asm__ volatile("addq $0x08, %rax");
    //     serial_printf("%x: 0x%x\n", addr, val );
    // }

    __asm__ volatile("movq %%rsp, %0" : "=r"( new_task->stack ) );  // get back new rsp
    __asm__ volatile("movq %0, %%rsp" :: "r"(old_rsp));

    struct task_info_t* crt = &kernel_task;
    while( crt->next != NULL )
        crt = crt->next;

    crt->next = new_task;
}

void task_exit( qword signal ) {
    dword id_task = task_getid();
    if( id_task == 0 )
        return;     // for kernel, we cannot be exit of the task because of it main task
    
    struct task_info_t* target = task_current;
    target->signal_code = signal;
    target->flags = target->flags & ~(TASK_FLAGS_RUNNING_BIT);

    while( 1 );
}

// void task_remove( ) {
//     __asm__ volatile("mov %0, %%rsp" :: "r"( (qword) task_stack ) );
//     struct task_info_t* target = task_current;
//     printf("Task exit with Signal Code (%x)\n", target->signal_code );

//     struct task_info_t* prev = &kernel_task;
//     struct task_info_t* next = target->next;
//     while( prev->next != target )
//     {
//         prev = prev->next;
//         assert( prev != NULL );         // system stop cause prev is NULL
//     }

//     prev->next = task_next;

//     page_umapv( target->stack_provider, 0x1000 );
//     free_alloc( target );

//     switch_next_task_imme();
// }

byte is_ctask_running() {
    return (task_current->flags & TASK_FLAGS_RUNNING_BIT) != 0 ;
}

void task_print() {
    printf("Current Task: %x, Next Task: %x\n", task_current, task_next );
    printf("List Task:\n");
    struct task_info_t* crt = &kernel_task;
    int index = 0;
    while( crt != NULL )
    {
        printf("(%x) Task %i: rsp=%x, flags=%x, next=%x\n", crt, index, crt->stack, crt->flags, crt->next );
        crt = crt->next;
        index++;
    }
}

void __tprint() {
    printf("Current Task: %x, Next Task: %x\n", task_current, task_next );
}
