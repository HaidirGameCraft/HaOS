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

dword* page_directory = (dword*) PAGEDIR_ADDRESS;
dword* higher_half_page_table = (dword*) PAGEDIR_HALFKERNEL;
dword* frame_page_table = (dword*) PAGEDIR_FRAMETABLE;

dword page_getframe();
void  page_mapframe( dword address );
void  page_umapframe();

void page_init() {
    
    // We make paging to higher half kernel
    dword kernel_start = (dword) __kernel_start;
    dword kernel_end = (dword) __kernel_end;
    dword kernel_size = kernel_end - kernel_start;
    
    // from Bootloader need to assign the page_table for higher half kernel
    int _kernelpageSize = (int)( kernel_size / PAGE_SIZE ) + ( kernel_size % PAGE_SIZE > 0 );
    
    dword* _f = (dword*) PAGE_ADDRESS(1023, 0);
    // page_mapframe( (dword) higher_half_page_table );
    // memzero( (void*) _f, 1024 * sizeof( dword ) );
    
    // for( int i = 0; i < _kernelpageSize; i++ ) {
    //     dword addr = kernel_start + i * PAGE_SIZE;
    //     dword phy_addr = addr - 0xC0000000;
    //     _f[PAGETABLE_INDEX(addr)] = phy_addr | (1 << 0) | (1 << 1);
    //     int off = (int)((phy_addr >> 12) / 8);
    //     int shf = (phy_addr >> 12) % 8;
    //     // serial_printf("%x:%x = %x\n", addr, phy_addr, _f[PAGETABLE_INDEX(addr)]);
    //     bitmap[off] |= 1 << shf;
    // }
    // page_umapframe();
    // page_directory[ PAGEDIR_INDEX( kernel_start ) ] = ((dword) higher_half_page_table) | (1 << 0) | (1 << 1);

    // serial_printf("Page Dir Index: %x\n", PAGEDIR_INDEX( kernel_start ) );
    // print("Create and Mapped kernel into Page Successfull!\n");
    
    for( int i = 0; i < PAGE_ENTRIES - 1; i++ ) {
        if( page_directory[i] == 0 ) continue;
        serial_printf("Page Directory[%i]: %x\n", i, page_directory[i] );
        dword* page_table = (dword*)( page_directory[i] & ~0xFFF );
        page_mapframe( (dword) page_table );
        for( int j = 0; j < PAGE_ENTRIES; j++ ) {
            if( _f[j] == 0 ) continue;
            serial_printf("     Page Table[%i]: %x\n", j, _f[j] );
        }
        page_umapframe();
    }
    
    page_enable((dword) page_directory);
    page_directory[1023] = ((dword) frame_page_table) | (1 << 0) | ( 1 << 1 );
    serial_printf("Paging has been enable!\n");
}

dword page_getframe() {
    // Find the available physical address to do mapped
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

    // make flush virtual address due to avoid from map previous page
    __asm__ volatile("invlpg (%0)" :: "r"( PAGE_ADDRESS( 1023, 0 )));
}
void  page_umapframe() {
    frame_page_table[0] = 0;
    // make flush virtual address due to avoid from map previous page
    __asm__ volatile("invlpg (%0)" :: "r"( PAGE_ADDRESS( 1023, 0 )));
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
        dword addr = page_getframe();;
        page_mapframe( (dword)( page_directory[PAGEDIR_INDEX(_v)] & ~0xFFF ));
        _f[PAGETABLE_INDEX(_v)] = ((dword) addr) | (1 << 0) | (1 << 1);
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

dword page_getSizeMemoryUse() {
    dword size = 0;
    for( int i = 0; i < PAGE_BITMAP_SIZE; i++ ) {
        if( bitmap[i] == 0xFF ) {
            size += 0x1000 * 8;
            continue;
        }

        for( int j = 0; j < 8; j++ )
            if( ((bitmap[i] >> j) & 1) )
                size += 0x1000;
    }

    return size;
}