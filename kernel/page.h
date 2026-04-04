#pragma once

#include <type.h>

#define PAGE_PRESENT        ( 1 << 0 )
#define PAGE_READWRITE      ( 1 << 1 )
#define PAGE_USERSPACE      ( 1 << 2 )

void page_init();

// - creating new paging PMLT4, PDPT and PDT
qword* page_create();
void page_setPMLT4( qword pmlt4 );
void page_setPMLT4Default();
void page_installMainPage();

qword page_alloc4K( word flags );
qword page_map( qword length, word flags );
void page_mapv( qword virt, qword length, word flags );
void page_mapvp( qword virt, qword phys, qword length, word flags );

// unmapped page
void page_umapv( qword virt, qword length );

void page_print();


qword page_getSizeMemoryUse();

extern void page_enable( qword page_dir );
