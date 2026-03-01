#include <page.h>
#include <string.h>
#include <serial.h>
#include <config.h>

#define PAGE_SIZE       0x1000
#define PAGE_ENTRIES    1024
#define PAGEDIR_INDEX(x)    (x >> 22 & (PAGE_ENTRIES - 1))
#define PAGETABLE_INDEX(x)  (x >> 12 & (PAGE_ENTRIES - 1))
#define PAGE_ADDRESS(x, y)  ((x << 22) | (y << 12))
#define ALIGN_4K __attribute__((aligned(0x1000)))

byte* bitmap = ( byte* ) PAGE_BITMAP_ADDRESS;

dword page_directory[1024] ALIGN_4K;
dword first_page_table[1024] ALIGN_4K;
dword frame_page_table[1024] ALIGN_4K;

dword page_getframe();
void  page_mapframe( dword address );
void  page_umapframe();

void page_init() {
    memzero( (void*) page_directory, 1024 * sizeof( dword ) );
    memzero( (void*) first_page_table, 1024 * sizeof( dword ) );
    memzero( (void*) frame_page_table, 1024 * sizeof( dword ) );

    for( int i = 0; i < PAGE_ENTRIES; i++ ) {
        first_page_table[i] = ( i * PAGE_SIZE ) | (1 << 0) | (1 << 1);
        bitmap[(dword)(i / 8)] |= 1 << (i % 8);
    }
    page_directory[0] = ((dword) first_page_table) | (1 << 0) | (1 << 1);
    page_directory[1023] = ((dword) frame_page_table) | (1 << 0) | ( 1 << 1 );
    // print("Create and Mapped kernel into Page Successfull!\n");

    page_enable((dword) page_directory);
    // print("Paging has been enable!\n");
}

dword page_getframe() {
    for( int i = 0; i < PAGE_BITMAP_SIZE; i++ )
    {
        if( bitmap[i] == 0xFF )
            continue;

        for( int j = 0; j < 8; j++ )
        {
            if( !((bitmap[i] >> j) & 1) )
            {
                bitmap[i] |= 1 << j;
                dword addr = ( j + i * 8 ) * PAGE_SIZE;
                page_mapframe( addr );
                memzero( (void*) PAGE_ADDRESS(1023, 0), PAGE_SIZE );
                page_umapframe();
                return ( j + i * 8 ) * PAGE_SIZE;
            }
        }
    }

    return -1;
}

void  page_mapframe( dword address ) {
    frame_page_table[0] = ( address ) | (1 << 0) | ( 1 << 1 );
}
void  page_umapframe() {
    frame_page_table[0] = 0;
}

void page_map( dword length ) {
    // TODO
}

void page_mapv( dword virt, dword length ) {
    size_t size = (dword)( length / PAGE_SIZE ) + ( length % PAGE_SIZE > 0 );
    dword* _f = (dword*) PAGE_ADDRESS(1023, 0);
    for( int i = 0; i < size; i++ ) {
        addr_t _v = virt + i * PAGE_SIZE;

        if( !(page_directory[PAGEDIR_INDEX(_v)] & (1 << 0)) )
        {
            dword page = page_getframe();
            page_directory[ PAGEDIR_INDEX( _v ) ] = ( page ) | (1 << 0) | (1 << 1);
        }
        page_mapframe( (dword)( page_directory[PAGEDIR_INDEX(_v)] & ~0xFFF ));
        if( _f[PAGETABLE_INDEX(_v)] != 0 ) continue;
        _f[PAGETABLE_INDEX(_v)] = ((dword) page_getframe()) | (1 << 0) | (1 << 1);
    }
    page_umapframe();
}
void page_mapvp( dword virt, dword phys, dword length ) {
    size_t size = (dword)( length / PAGE_SIZE ) + ( length % PAGE_SIZE > 0 );
    dword* _f = (dword*) PAGE_ADDRESS(1023, 0);
    for( int i = 0; i < size; i++ ) {
        addr_t _v = virt + i * PAGE_SIZE;
        addr_t _p = phys + i * PAGE_SIZE;

        if( !(page_directory[PAGEDIR_INDEX(_v)] & (1 << 0)) )
        {
            dword page = page_getframe();
            page_directory[ PAGEDIR_INDEX( _v ) ] = ( page ) | (1 << 0) | (1 << 1);
        }
        page_mapframe( (dword)( page_directory[PAGEDIR_INDEX(_v)] & ~0xFFF ));
        if( _f[PAGETABLE_INDEX(_v)] != 0 ) continue;
        _f[PAGETABLE_INDEX(_v)] = ((dword) _p) | (1 << 0) | (1 << 1);
    }
    page_umapframe();
}