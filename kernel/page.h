#pragma once

#include <type.h>

void page_init();
void page_map( dword length );
void page_mapv( dword virt, dword length );
void page_mapvp( dword virt, dword phys, dword length );

extern void page_enable( dword page_dir );