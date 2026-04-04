#include <type.h>
#include "cpu.h"

#include <alloc.h>
#include <string.h>
#include <fs/fat.h>
#include <io.h>

#include <process/task.h>

/**
 * void syscall_handle( cpu_register_t cpu ) - request or receive system service of kernel from Userspace
 *
 *
 */
qword syscall_handle( cpu_register_t cpu ) {
    if( cpu.rax == 0 ) // read syscall
    {
        // rdi - fat_fileptr_t* file
        // rsi - char* buffer
        // rdx - size_t size
        fat_read( (fat_file_ptr*) cpu.rdi, (char*) cpu.rsi, cpu.rdx );
    }  else if ( cpu.rax == 1 ) // write syscall
    {
        // NOT IMPLEMENT YET...
    } else if ( cpu.rax == 2 ) // open syscall - open file
    {
        // rdi - const char* filename
        fat_file_ptr fileptr =  fat_fileOpen( ( const char* ) cpu.rdi );
        fat_file_ptr* __fp = ( fat_file_ptr* ) new_alloc( sizeof( fat_file_ptr ) );
        memcopy( __fp, &fileptr, sizeof( fat_file_ptr ) );
        cpu.rax = (qword) __fp;
    } else if ( cpu.rax == 3 ) // close syscall - deleting pointer of file on memory
    {
        // rdi - fat_file_ptr* fptr
        free_alloc( (void*) cpu.rdi );
    } else if ( cpu.rax == 4 ) // displayng pixel
    {
        // rdi - dword x
        // rsi - dword y
        // rdx = dword color
        extern void video_driver_putPixel( int x, int y, dword color );
        video_driver_putPixel((dword) cpu.rdi, (dword) cpu.rsi, (dword) cpu.rdx );
        cpu.rax = 0;
    } else if ( cpu.rax == 5 ) // put character to term
    {
        //rdi - char __c__
        extern void term_putc( char __c__ );
        term_putc( (char) cpu.rdi );
    }
    else if ( cpu.rax == 6 )    // exit - quit Task
    {
        // rdi - signal
        
        task_exit( (qword) cpu.rdi );
    }

    return cpu.rax;
}
