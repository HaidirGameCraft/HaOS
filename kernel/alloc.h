#pragma once
/*
    alloc - Memory Management
    - design to serve memory with specific size from user
    - this method is same like Linux ( malloc )
    Code: created by Haidir
*/

#include <type.h>

void  init_alloc();
void* new_alloc( size_t size );
void  free_alloc( void* ptr );