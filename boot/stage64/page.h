#pragma once

#include <type.h>

void page_init();
void page_map( qword length );
void page_mapv( qword virt, qword length, word flags );
void page_mapvp( qword virt, qword phys, qword length, word flags );

void page_showList();

extern void page_enable( qword page_dir );
