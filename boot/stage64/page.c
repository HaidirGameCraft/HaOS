#include <page.h>
#include "string.h"
#include <serial.h>
#include "cpu.h"
#include "io.h"
#include "config.h"
#include "../../kernel/config.h"

#define PAGE_BITMAP_ADDRESS     0x1000
#define PMLT4_ADDRESS           0x2000
#define PDPT_ADDRESS            0x3000
#define PDT_ADDRESS             0x4000
#define FIRST_PT_ADDRESS        0x5000
#define FRAME_PT_ADDRESS        0x6000

#define PAGE_SIZE       0x1000
#define PAGE_ENTRIES    512
#define PAGE_BITMAP_SIZE 0x1000


#define PMLT4_INDEX(x)    (x >> 39 & (PAGE_ENTRIES - 1))
#define PDPT_INDEX(x)    (x >> 30 & (PAGE_ENTRIES - 1))
#define PAGEDIR_INDEX(x)    (x >> 21 & (PAGE_ENTRIES - 1))
#define PAGETABLE_INDEX(x)  (x >> 12 & (PAGE_ENTRIES - 1))
#define PAGE_ADDRESS(pmlt4, pdpt, pd, pt)  ((pmlt4 << 39) | (pdpt << 30) | (pd << 21) | (pt << 12) )



#define ALIGN_4K __attribute__((aligned(0x1000)))

byte* bitmap = ( byte* ) PAGE_BITMAP_ADDRESS;

qword* pmlt4 = ( qword* ) PMLT4_ADDRESS;
qword* pdpt = ( qword* ) PDPT_ADDRESS;
qword* pdt = ( qword* ) PDT_ADDRESS;
qword* first_pt = ( qword* ) FIRST_PT_ADDRESS;
qword* frame_page_table = ( qword* ) FRAME_PT_ADDRESS;

qword page_getframe();
void  page_mapframe( qword address );
void  page_umapframe();

void  page_mapframei( qword address, qword index );
void  page_umapframei( qword index );

void page_init() {
    memory_zero( (void*) bitmap, 0x1000 );
    qword size = (qword) __BOOTSTAGE_END - (qword) __BOOTSTAGE_START;
    qword len_boot = ((qword)(size / 0x1000)) + (size % 0x1000 > 0);
    // set ther bitmap for bitmap, page_directory, first_page_table and frame_page_table

    dword fnal_index = PAGETABLE_INDEX( (qword) __BOOTSTAGE_END );

    for( qword i = 0; i <= fnal_index; i++ )
        bitmap[(qword)(i / 8)] |= 1 << (i % 8);

    for( qword i = fnal_index + 1; i < 512; i++ )
        first_pt[i] = 0;        // Clear the waste of physical address


    // page_enable((qword) pmlt4);
    // print("Paging has been enable!\n");
}

qword page_getframe() {
    for( qword i = 0; i < PAGE_BITMAP_SIZE; i++ )
    {
        if( bitmap[i] == 0xFF )
            continue;

        for( qword j = 0; j < 8; j++ )
        {
            if( !((bitmap[i] >> j) & 1) )
            {
                bitmap[i] |= 1 << j;
                qword addr = ( j + i * 8 ) * PAGE_SIZE;
                page_mapframe( addr );
                memory_zero( (void*) PAGE_ADDRESS(0, 0, 511, 0), PAGE_SIZE );
                page_umapframe( );
                return addr;
            }
        }
    }

    return -1;
}

void  page_mapframe( qword address ) {
    frame_page_table[0] = ( address ) | (1 << 0) | ( 1 << 1 );

    // Flush the address after mapping of frame due to avoid from mapped the previous one again
    __asm__ volatile("invlpg (%0)" :: "r"(PAGE_ADDRESS(0, 0, 511, 0)));
}
void  page_umapframe() {
    frame_page_table[0] = 0;

    // Flush the address after mapping of frame due to avoid from mapped the previous one again
    __asm__ volatile("invlpg (%0)" :: "r"(PAGE_ADDRESS(0, 0, 511, 0)));
}


void  page_mapframei( qword address, qword index ) {
    frame_page_table[ index ] = ( address ) | ( 1 << 0 ) | ( 1 << 1 );
}
void  page_umapframei( qword index ) {
    frame_page_table[ index ] = 0;
}

//void page_map( qword length ) {
//    // TODO
//}

void page_mapv( qword virt, qword length, word flags ) {
    size_t size = (qword)( length / PAGE_SIZE ) + ( length + PAGE_SIZE > 0 );
    for( qword i = 0; i < size; i++ ) {
        addr_t _v = virt + i * PAGE_SIZE;

        qword pmlt4_index = PMLT4_INDEX( _v );
        qword pdpt_index = PDPT_INDEX( _v );
        qword pdt_index = PAGEDIR_INDEX( _v );
        qword pt_index = PAGETABLE_INDEX( _v );

        if( !(pmlt4[pmlt4_index] & 1 ) )
        {
            qword page = page_getframe();
            pmlt4[ pmlt4_index ] = page | ( 1 << 0 ) | ( 1 << 1 );
        }

        qword __tpmlt4 = (qword)( pmlt4[pmlt4_index] & ~0x1FF );
        page_mapframe( __tpmlt4 );
        qword* _pdpt = (qword*) PAGE_ADDRESS(0, 0, 511, 0 );

        if( !(_pdpt[pdpt_index] & 1 ) )
        {
            qword page = page_getframe();
            page_mapframe( __tpmlt4 );
            _pdpt[ pdpt_index ] = page | ( 1 << 0 ) | ( 1 << 1 );
        }

        qword __tpdpt = (qword)( _pdpt[pdpt_index] & ~0x1FF );
        page_mapframe( __tpdpt );
        qword* _pdt = (qword*) PAGE_ADDRESS(0, 0, 511, 0 );

        if( !(_pdt[pdt_index] & 1 ) )
        {
            qword page = page_getframe();
            page_mapframe( __tpdpt ); // <=
            _pdt[ pdt_index ] = page | ( 1 << 0 ) | ( 1 << 1 );
        }

        qword __tpdt = (qword)( _pdt[pdt_index] & ~0x1FF );
        page_mapframe( __tpdt );
        qword* _pt = (qword*) PAGE_ADDRESS(0, 0, 511, 0 );

        qword phy = page_getframe();
        page_mapframe( __tpdt );

        if( _pt[ pt_index] != 0 )
        {
            page_umapframe();
            continue;
        }

        _pt[ pt_index ] = phy | flags;


        page_umapframe();
    }
}

void page_mapvp( qword virt, qword phys, qword length, word flags ) {
    size_t size = (qword)( length / PAGE_SIZE ) + ( length + PAGE_SIZE > 0 );
    for( qword i = 0; i < size; i++ ) {
        addr_t _v = virt + i * PAGE_SIZE;
        addr_t _p = phys + i * PAGE_SIZE;

        qword pmlt4_index = PMLT4_INDEX( _v );
        qword pdpt_index = PDPT_INDEX( _v );
        qword pdt_index = PAGEDIR_INDEX( _v );
        qword pt_index = PAGETABLE_INDEX( _v );

        if( !(pmlt4[pmlt4_index] & 1 ) )
        {
            qword page = page_getframe();
            pmlt4[ pmlt4_index ] = page | ( 1 << 0 ) | ( 1 << 1 );
        }

        qword __tpmlt4 = (qword)( pmlt4[pmlt4_index] & ~0x1FF );
        page_mapframe( __tpmlt4 );
        qword* _pdpt = (qword*) PAGE_ADDRESS(0, 0, 511, 0 );

        if( !(_pdpt[pdpt_index] & 1 ) )
        {
            qword page = page_getframe();
            page_mapframe( __tpmlt4 );
            _pdpt[ pdpt_index ] = page | ( 1 << 0 ) | ( 1 << 1 );
        }

        qword __tpdpt = (qword)( _pdpt[pdpt_index] & ~0x1FF );
        page_mapframe( __tpdpt );
        qword* _pdt = (qword*) PAGE_ADDRESS(0, 0, 511, 0 );

        if( !(_pdt[pdt_index] & 1 ) )
        {
            qword page = page_getframe();
            page_mapframe( __tpdpt ); // <=
            _pdt[ pdt_index ] = page | ( 1 << 0 ) | ( 1 << 1 );
        }

        qword __tpdt = (qword)( _pdt[pdt_index] & ~0x1FF );
        page_mapframe( __tpdt );
        qword* _pt = (qword*) PAGE_ADDRESS(0, 0, 511, 0 );

        if( _pt[ pt_index] != 0 )
        {
            page_umapframe();
            continue;
        }

        bitmap[ PAGETABLE_INDEX( _p ) / 8 ] |= 1 << (PAGETABLE_INDEX( _p ) % 8);
        _pt[ pt_index ] = _p | flags;


        page_umapframe();
    }
}

void page_showList() {
    qword* table = ( qword* ) PAGE_ADDRESS(0, 0, 511, 0 );
    for( int i = 0; i < PAGE_ENTRIES; i++ )
    {
        if( pmlt4[i] == 0 )
            continue;

        printf("PMLT4[%i](%x): %x\n", i, PAGE_ADDRESS(i, 0, 0, 0), pmlt4[i]);

        // PDPT
        for( int j = 0; j < PAGE_ENTRIES; j++ ) {
            page_mapframe( (qword)( pmlt4[i] & ~0x1FF ) );
            if( table[j] == 0 ) continue;

            // PMLT4 -> PDPT
            qword __pdpt = (qword)( table[j] & ~0x1FF );
            printf("--PDPT[%i](%x): %x\n", j, PAGE_ADDRESS(i, j, 0, 0), table[j]);

            // PDT
            for( int m = 0; m < PAGE_ENTRIES; m++ ) {
                page_mapframe( __pdpt );
                if( table[m] == 0 ) continue;

                // PDPT -> PDT
                qword __pdt = ( qword )( table[m] & ~0x1FF );
                printf("----PDT[%i](%x): %x\n", m, PAGE_ADDRESS(i, j, m, 0), table[m]);

                if( i == 0 && j == 0 && m == PAGE_ENTRIES - 1 )
                {
                    printf("        // Private Frame Page\n");
                    continue;
                }

                // PT
                for( int k = 0; k < PAGE_ENTRIES; k++ ) {
                    page_mapframe( __pdt );
                    if( table[k] == 0 ) continue;

                    // PDT -> PT
                    // qword __pt = ( qword )( table[k] & ~0x1FF );
                    printf("------PT[%i](%x): %x\n", k, PAGE_ADDRESS(i, j, m, k), table[k]);

                }
            }
        }
    }
}

void page_fault_handle( cpu_register_t* reg )
{

    qword cr2 = 0;
    // __asm__ volatile( "movl %%cr2,%0" : "=r"(cr2) );

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
