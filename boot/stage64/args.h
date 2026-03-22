#pragma once

#include "type.h"

#define CREATE_ARGS() qword args[6];

void get_args( dword index, qword* arg ) {
    __asm__ volatile("mov %%rdi, %0" : "=r"(arg[0]) );
    __asm__ volatile("mov %%rsi, %0" : "=r"(arg[1]) );
    __asm__ volatile("mov %%rdx, %0" : "=r"(arg[2]) );
    __asm__ volatile("mov %%rcx, %0" : "=r"(arg[3]) );
    __asm__ volatile("mov %%r8, %0"  : "=r"(arg[4]) );
    __asm__ volatile("mov %%r9, %0"  : "=r"(arg[5]) );
}