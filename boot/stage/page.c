#include <page.h>
#include "string.h"
#include <serial.h>
#include "cpu.h"
#include "io.h"
#include "config.h"
#include "../../kernel/config.h"

#define PAGE_SIZE       0x1000
#define PAGE_ENTRIES    1024
#define PAGE_BITMAP_SIZE 0x1000
#define PAGEDIR_INDEX(x)    (x >> 22 & (PAGE_ENTRIES - 1))
#define PAGETABLE_INDEX(x)  (x >> 12 & (PAGE_ENTRIES - 1))
#define PAGE_ADDRESS(x, y)  ((x << 22) | (y << 12))
#define ALIGN_4K __attribute__((aligned(0x1000)))

byte* bitmap = ( byte* ) PAGEBITMAP_ADDRESS;

dword* page_directory = (dword*) PAGEDIR_ADDRESS;
dword* first_page_table = (dword*) PAGEDIR_FIRSTTABLE;
dword* frame_page_table = (dword*) 0x4000;

dword page_getframe();
void  page_mapframe( dword address );
void  page_umapframe();

void  page_mapframei( dword address, int index );
void  page_umapframei( int index );

void page_init() {
    memory_zero( (void*) page_directory, 1024 * sizeof( dword ) );
    memory_zero( (void*) first_page_table, 1024 * sizeof( dword ) );
    memory_zero( (void*) frame_page_table, 1024 * sizeof( dword ) );

    dword size = (dword) __BOOTSTAGE_END - (dword) __BOOTSTAGE_START;
    dword len_boot = ((dword)(size / 0x1000)) + (size % 0x1000 > 0);
    // set ther bitmap for bitmap, page_directory, first_page_table and frame_page_table

    first_page_table[PAGETABLE_INDEX((dword) bitmap)] = ((dword) bitmap) | (1 << 0) | (1 << 1);
    first_page_table[PAGETABLE_INDEX((dword) page_directory)] = ((dword) page_directory) | (1 << 0) | (1 << 1);
    first_page_table[PAGETABLE_INDEX((dword) first_page_table)] = ((dword) first_page_table) | (1 << 0) | (1 << 1);
    first_page_table[PAGETABLE_INDEX((dword) frame_page_table)] = ((dword) frame_page_table) | (1 << 0) | (1 << 1);

    bitmap[0] = 0b11111;

    for( int i = 0; i < len_boot; i++ ) {
        dword phy = ((dword) __BOOTSTAGE_START & ~0xFFF) + i * PAGE_SIZE;
        first_page_table[PAGETABLE_INDEX(phy)] = phy | (1 << 0) | (1 << 1);
        bitmap[(dword)(PAGETABLE_INDEX(phy) / 8)] |= 1 << (PAGETABLE_INDEX(phy) % 8);
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
                memory_zero( (void*) PAGE_ADDRESS(1023, 0), PAGE_SIZE );
                page_umapframe( );
                return addr;
            }
        }
    }

    return -1;
}

void  page_mapframe( dword address ) {
    frame_page_table[0] = ( address ) | (1 << 0) | ( 1 << 1 );

    // Flush the address after mapping of frame due to avoid from mapped the previous one again
    __asm__ volatile("invlpg (%0)" :: "r"(PAGE_ADDRESS(1023, 0)));
}
void  page_umapframe() {
    frame_page_table[0] = 0;

    // Flush the address after mapping of frame due to avoid from mapped the previous one again
    __asm__ volatile("invlpg (%0)" :: "r"(PAGE_ADDRESS(1023, 0)));
}


void  page_mapframei( dword address, int index ) {
    frame_page_table[ index ] = ( address ) | ( 1 << 0 ) | ( 1 << 1 );
}
void  page_umapframei( int index ) {
    frame_page_table[ index ] = 0;
}

void page_map( dword length ) {
    // TODO
}

void page_mapv( dword virt, dword length ) {
    size_t size = (dword)( length / PAGE_SIZE ) + ( length % PAGE_SIZE > 0 );
    for( int i = 0; i < size; i++ ) {
        addr_t _v = virt + i * PAGE_SIZE;
        
        if( !(page_directory[PAGEDIR_INDEX(_v)] & (1 << 0)) )
        {
            dword page = page_getframe();
            page_directory[ PAGEDIR_INDEX( _v ) ] = ( page ) | (1 << 0) | (1 << 1);
            printf("new\n");
        }
        page_mapframe( (dword)( page_directory[PAGEDIR_INDEX(_v)] & ~0xFFF ) );
        dword* _f = (dword*) PAGE_ADDRESS(1023, 0);
        if( _f[PAGETABLE_INDEX(_v)] != 0 ) {
            page_umapframe( );
            continue;
        }
        
        // printf("[page_mapv (Before)]: Map Frame Address: %x\n", frame_page_table[0] );
        dword addr = page_getframe();
        page_mapframe( (dword)( page_directory[PAGEDIR_INDEX(_v)] & ~0xFFF ) );
        // printf("[page_mapv (After)]: Map Frame Address: %x\n", frame_page_table[0] );
        // printf("%x, Virt: %x, Addr: %x\n", _v, _f[PAGETABLE_INDEX(_v) - 1], addr);
        _f[PAGETABLE_INDEX(_v)] = addr | (1 << 0) | (1 << 1);
        page_umapframe( );
    }
}

void page_mapvp( dword virt, dword phys, dword length ) {
    size_t size = (dword)( length / PAGE_SIZE ) + ( length % PAGE_SIZE > 0 );
    for( int i = 0; i < size; i++ ) {
        addr_t _v = virt + i * PAGE_SIZE;
        addr_t _p = phys + i * PAGE_SIZE;
        
        if( !(page_directory[PAGEDIR_INDEX(_v)] & (1 << 0)) )
        {
            dword page = page_getframe();
            page_directory[ PAGEDIR_INDEX( _v ) ] = ( page ) | (1 << 0) | (1 << 1);
            printf("New Page Tables at 0x%x\n", page);
        }
        page_mapframei( (dword)( page_directory[PAGEDIR_INDEX(_v)] & ~0xFFF ), 1);
        // printf("Page Directory: 0x%x\n", page_directory[PAGEDIR_INDEX(_v)] );
        dword* _f = (dword*) PAGE_ADDRESS(1023, 1);
        if( _f[PAGETABLE_INDEX(_v)] != 0 ) continue;
        _f[PAGETABLE_INDEX(_v)] = ((dword) _p) | (1 << 0) | (1 << 1);
        // printf("Virt: 0x%x, PageDir: 0x%x, PageTable: 0x%x, value: 0x%x\n", _v, PAGEDIR_INDEX( _v) , PAGETABLE_INDEX( _v ), _f[PAGETABLE_INDEX(_v)] );
        page_umapframei( 1 );
    }
}

void page_showList() {
    for( int i = 0; i < PAGE_ENTRIES; i++ )
    {
        dword* page_table = (dword*)( page_directory[i] & ~0xFFF );
        if( page_table == 0 ) continue;

        page_mapframe( (dword) page_table );
        printf("Page Directory[%i]: %x\n", i, page_table );
        page_table = (dword*) PAGE_ADDRESS(1023, 0);
        for( int j = 0; j < PAGE_ENTRIES; j++ ) {
            if( page_table[j] == 0 ) continue;
            
            printf("    Page Table[%i]: %x\n", j, page_table[j]);
        }
        page_umapframe( );
    }
}

void page_fault_handle( cpu_register_t* reg )
{

    dword cr2 = 0;
    __asm__ volatile( "mov %%cr2,%0" : "=r"(cr2) );

    printf("Address Error: 0x%x\n", cr2 );

    printf("eax: 0x%x\n", reg->eax );
    printf("ecx: 0x%x\n", reg->ecx );
    printf("edx: 0x%x\n", reg->edx );
    printf("ebx: 0x%x\n", reg->ebx );
    printf("esp: 0x%x\n", reg->esp );
    printf("ebp: 0x%x\n", reg->ebp );
    printf("esi: 0x%x\n", reg->esi );
    printf("edi: 0x%x\n", reg->edi );
    printf("eflags: 0x%x\n", reg->eflags );
    printf("interrupt code: 0x%x\n", reg->interrupt_code );
    printf("error code: 0x%x\n", reg->error_code );

    // Halt the program;
    for(;;)
    {
        __asm__ volatile("cli; hlt;");
    }
}