#pragma once

#include <type.h>

void  init_alloc();
void* new_alloc( dword size );
void  free_alloc( void* ptr );