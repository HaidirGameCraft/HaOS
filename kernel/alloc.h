#pragma once

#include <type.h>

void  init_alloc();
void* new_alloc( size_t size );
void  free_alloc( void* ptr );