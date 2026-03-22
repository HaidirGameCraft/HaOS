#include <page.h>
#include <string.h>
#include <serial.h>
#include <config.h>

#define PAGE_SIZE       0x1000
#define PAGE_ENTRIES    512


#define PMLT4_INDEX(x)      (x >> 39 & (PAGE_ENTRIES - 1))
#define PDPT_INDEX(x)       (x >> 30 & (PAGE_ENTRIES - 1))
#define PDT_INDEX(x)        (x >> 21 & (PAGE_ENTRIES - 1))
#define PT_INDEX(x)         (x >> 12 & (PAGE_ENTRIES - 1))

#define PAGE_ADDRESS(pml4, pdpt, pdt, pt) ( ( (qword)(pml4 & 0x1FF) << 39 ) | ( (qword)(pdpt & 0x1FF ) << 30 ) | ( (qword)(pdt & 0x1FF) << 21 ) | ( (qword)(pt & 0x1FF) << 12 ) )
#define ALIGN_4K __attribute__((aligned(0x1000)))

byte* bitmap = ( byte* ) PAGE_BITMAP_ADDRESS;

qword* pmlt4 = ( qword* ) PMLT4_ADDRESS;
qword* pdpt = ( qword* ) PDPT_ADDRESS;
qword* pdt = ( qword* ) PDT_ADDRESS;
qword* first_pt = ( qword* )  FIRST_PT_ADDRESS;
qword* frame_pt = ( qword* ) FRAME_PT_ADDRESS;

qword page_getframe();
void  page_mapframe( qword address );
void  page_umapframe();

void page_init() {
    
    // We make paging to higher half kernel
    void* kernel_start = (void*) &__kernel_start;
    void* kernel_end = __kernel_end;
    qword kernel_size = kernel_end - kernel_start;
    
    // from Bootloader need to assign the page_table for higher half kernel
    int _kernelpageSize = (int)( kernel_size / PAGE_SIZE ) + ( kernel_size % PAGE_SIZE > 0 );
    // Noting TODO Right here
    serial_printf("Paging has been enable!\n");
}

qword* page_create() {

    // Simple and easy, just get the available frame
    qword* __pmlt4 = ( qword* ) page_alloc4K();

    // Alloc as First Page Table
    qword* __pdpt = ( qword* ) page_alloc4K();
    qword* __pdt = ( qword* ) page_alloc4K();
    qword* __pt = ( qword* ) page_alloc4K();

    page_mapframe((qword) __pt );
    __pmlt4[0] = ((qword) __pdpt) | 1 << 1 | 1 << 0;
    __pdpt[0] = ((qword) __pdt) | 1 << 1 | ! 1 << 0;
    __pdt[0] = ((qword) __pt) | 1 << 1 | 1 << 0;

    __pt[0] = PAGE_BITMAP_ADDRESS | 1 << 1 | 1 << 0;

    // we need to set main pmlt4 into __pmlt4 including ( framebuffer, frame and  kernel)
    qword* higher_table_pmlt4 = ( qword* ) ((qword) pmlt4[ PMLT4_INDEX( KERNEL_HIGHER_HALF_KKERNEL ) ] & ~0x1FF );
    __pmlt4[ PMLT4_INDEX(KERNEL_HIGHER_HALF_KKERNEL) ] = ((qword) higher_table_pmlt4) | 1 << 1 | 1 << 0; 

    return __pmlt4;
}

void page_setPMLT4( qword __pmlt4 )  {
    pmlt4 = ( qword* ) __pmlt4;
}
void page_setPMLT4Default() {
    pmlt4 = ( qword* ) PMLT4_ADDRESS;
}

void page_print() {
    qword* table = ( qword* ) PAGE_ADDRESS(0, 0, 511, 0 );
    for( int i = 0; i < PAGE_ENTRIES; i++ )
    {
        if( pmlt4[i] == 0 )
            continue;

        serial_printf("PMLT4[%i](%x): %x\n", i, PAGE_ADDRESS(i, 0, 0, 0), pmlt4[i]);

        // PDPT
        for( int j = 0; j < PAGE_ENTRIES; j++ ) {
            page_mapframe( (qword)( pmlt4[i] & ~0x1FF ) );
            if( table[j] == 0 ) continue;

            // PMLT4 -> PDPT
            qword __pdpt = (qword)( table[j] & ~0x1FF );
            serial_printf("--PDPT[%i](%x): %x\n", j, PAGE_ADDRESS(i, j, 0, 0), table[j]);

            // PDT
            for( int m = 0; m < PAGE_ENTRIES; m++ ) {
                page_mapframe( __pdpt );
                if( table[m] == 0 ) continue;

                // PDPT -> PDT
                qword __pdt = ( qword )( table[m] & ~0x1FF );
                serial_printf("----PDT[%i](%x): %x\n", m, PAGE_ADDRESS(i, j, m, 0), table[m]);

                if( i == 0 && j == 0 && m == PAGE_ENTRIES - 1 )
                {
                    serial_printf("        // Private Frame Page\n");
                    continue;
                }

                // PT
                for( int k = 0; k < PAGE_ENTRIES; k++ ) {
                    page_mapframe( __pdt );
                    if( table[k] == 0 ) continue;

                    // PDT -> PT
                    // qword __pt = ( qword )( table[k] & ~0x1FF );
                    serial_printf("------PT[%i](%x): %x\n", k, PAGE_ADDRESS(i, j, m, k), table[k]);

                }
            }
        }
    }
}

qword page_getframe() {
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
                qword addr = ( j + i * 8 ) * PAGE_SIZE;
                page_mapframe( addr );
                memzero( (void*) PAGE_ADDRESS(0, 0, 511, 0), PAGE_SIZE );
                page_umapframe();
                return ( j + i * 8 ) * PAGE_SIZE;
            }
        }
    }

    return -1;
}

void  page_mapframe( qword address ) {
    frame_pt[0] = ( address ) | (1 << 0) | ( 1 << 1 );

    // make flush virtual address due to avoid from map previous page
    __asm__ volatile("invlpg (%0)" :: "r"( PAGE_ADDRESS( 0, 0, 511, 0 )));
}
void  page_umapframe() {
    frame_pt[0] = 0;
    // make flush virtual address due to avoid from map previous page
    __asm__ volatile("invlpg (%0)" :: "r"( PAGE_ADDRESS( 0, 0, 511, 0 )));
}

qword page_alloc4K() {
    // TODO

    qword phys = page_getframe();

    // Initialize pmlt4, pdpt, pd and pt index
    int pmlt4_index = PMLT4_INDEX( phys );
    int pdpt_index = PDPT_INDEX( phys );
    int pdt_index = PDT_INDEX( phys );
    int pt_index = PT_INDEX( phys );


    // Determine and get the PDPT Address
    if( !( pmlt4[pmlt4_index] & PAGE_PRESENT ) )
    {
        // Find the available frame
        qword frame = page_getframe();
        pmlt4[pmlt4_index] = frame | PAGE_PRESENT | PAGE_READWRITE;
    }

    qword __pdpt_addr = (qword)( pmlt4[ pmlt4_index ] & ~0x1FF );
    page_mapframe( __pdpt_addr );
    qword* pdpt = ( qword* ) PAGE_ADDRESS(0, 0, 511, 0 );

    // Determina and get the PDT Address ( Page Directory Address )
    if( !( pdpt[pdpt_index] & PAGE_PRESENT ) )
    {
        qword frame = page_getframe();
        page_mapframe( __pdpt_addr );
        pdpt[pdpt_index] = frame | PAGE_PRESENT | PAGE_READWRITE;
    }

    qword __pdt_addr = ( qword )( pdpt[pdpt_index] & ~0x1FF );
    page_mapframe( __pdt_addr );
    qword* pdt = (qword*) PAGE_ADDRESS(0, 0, 511, 0);

    // Determine and get the PT Addreass ( Page Table address )
    if( !(pdt[pdt_index] & PAGE_PRESENT ) ) {
        qword frame = page_getframe();
        page_mapframe( __pdt_addr );
        pdt[pdt_index] = frame | PAGE_PRESENT | PAGE_READWRITE;
    }

    qword __pt_addr = ( qword )( pdt[pdt_index] & ~0x1FF );
    
    page_mapframe( __pt_addr );
    qword* pt = (qword*) PAGE_ADDRESS(0, 0, 511, 0);
    pt[pt_index] = phys | PAGE_PRESENT | PAGE_READWRITE;

    page_umapframe();
    return phys;
}

void page_mapv( qword virt, qword length ) {
    size_t size = (qword)( length / PAGE_SIZE ) + ( length % PAGE_SIZE > 0 );
    qword* _f = (qword*) PAGE_ADDRESS( 0, 0, 511, 0 );
    
    for( int i = 0; i < size; i++ )
    {

        qword __virt = virt + i * PAGE_SIZE;

        // Initialize pmlt4, pdpt, pd and pt index
        int pmlt4_index = PMLT4_INDEX( __virt );
        int pdpt_index = PDPT_INDEX( __virt );
        int pdt_index = PDT_INDEX( __virt );
        int pt_index = PT_INDEX( __virt );


        // Determine and get the PDPT Address
        if( !( pmlt4[pmlt4_index] & PAGE_PRESENT ) )
        {
            // Find the available frame
            qword frame = page_getframe();
            pmlt4[pmlt4_index] = frame | PAGE_PRESENT | PAGE_READWRITE;
        }

        qword __pdpt_addr = (qword)( pmlt4[ pmlt4_index ] & ~0x1FF );
        page_mapframe( __pdpt_addr );
        qword* pdpt = ( qword* ) PAGE_ADDRESS(0, 0, 511, 0 );

        // Determina and get the PDT Address ( Page Directory Address )
        if( !( pdpt[pdpt_index] & PAGE_PRESENT ) )
        {
            qword frame = page_getframe();
            page_mapframe( __pdpt_addr );
            pdpt[pdpt_index] = frame | PAGE_PRESENT | PAGE_READWRITE;
        }

        qword __pdt_addr = ( qword )( pdpt[pdpt_index] & ~0x1FF );
        page_mapframe( __pdt_addr );
        qword* pdt = (qword*) PAGE_ADDRESS(0, 0, 511, 0);

        // Determine and get the PT Addreass ( Page Table address )
        if( !(pdt[pdt_index] & PAGE_PRESENT ) ) {
            qword frame = page_getframe();
            page_mapframe( __pdt_addr );
            pdt[pdt_index] = frame | PAGE_PRESENT | PAGE_READWRITE;
        }

        qword __pt_addr = ( qword )( pdt[pdt_index] & ~0x1FF );
        // Get the available Address to do mapping
        qword phy_addr = page_getframe();
        
        page_mapframe( __pt_addr );
        qword* pt = (qword*) PAGE_ADDRESS(0, 0, 511, 0);
        pt[pt_index] = phy_addr | PAGE_PRESENT | PAGE_READWRITE;

        page_umapframe();
    }

}
void page_mapvp( qword virt, qword phys, qword length ) {
    size_t size = (qword)( length / PAGE_SIZE ) + ( length % PAGE_SIZE > 0 );
    
    for( int i = 0; i < size; i++ )
    {
        qword __virt = virt + i * PAGE_SIZE;
        qword __phys = phys + i * PAGE_SIZE;

        // Initialize pmlt4, pdpt, pd and pt index
        int pmlt4_index = PMLT4_INDEX( __virt );
        int pdpt_index = PDPT_INDEX( __virt );
        int pdt_index = PDT_INDEX( __virt );
        int pt_index = PT_INDEX( __virt );


        // Determine and get the PDPT Address
        if( !( pmlt4[pmlt4_index] & PAGE_PRESENT ) )
        {
            // Find the available frame
            qword frame = page_getframe();
            pmlt4[pmlt4_index] = frame | PAGE_PRESENT | PAGE_READWRITE;
        }

        qword __pdpt_addr = (qword)( pmlt4[ pmlt4_index ] & ~0x1FF );
        page_mapframe( __pdpt_addr );
        qword* pdpt = ( qword* ) PAGE_ADDRESS(0, 0, 511, 0 );

        // Determina and get the PDT Address ( Page Directory Address )
        if( !( pdpt[pdpt_index] & PAGE_PRESENT ) )
        {
            qword frame = page_getframe();
            page_mapframe( __pdpt_addr );
            pdpt[pdpt_index] = frame | PAGE_PRESENT | PAGE_READWRITE;
        }

        qword __pdt_addr = ( qword )( pdpt[pdpt_index] & ~0x1FF );
        page_mapframe( __pdt_addr );
        qword* pdt = (qword*) PAGE_ADDRESS(0, 0, 511, 0);

        // Determine and get the PT Addreass ( Page Table address )
        if( !(pdt[pdt_index] & PAGE_PRESENT ) ) {
            qword frame = page_getframe();
            page_mapframe( __pdt_addr );
            pdt[pdt_index] = frame | PAGE_PRESENT | PAGE_READWRITE;
        }

        qword __pt_addr = ( qword )( pdt[pdt_index] & ~0x1FF );
        page_mapframe( __pt_addr );
        qword* pt = (qword*) PAGE_ADDRESS(0, 0, 511, 0);

        if( pt[pt_index] != 0 )
        {
            page_umapframe();
            continue;
        }

        pt[pt_index] = __phys | PAGE_PRESENT | PAGE_READWRITE;
        page_umapframe();
    }
}

qword page_getSizeMemoryUse() {
    qword size = 0;
    for( int i = 0; i < PAGE_BITMAP_SIZE; i++ ) {
        byte __bit = bitmap[i];
        if( __bit == 0xFF ) {
            size += 0x1000 * 8;
            continue;
        }

        for( int j = 0; j < 8; j++ )
            if( ((__bit >> j) & 1) )
                size += 0x1000;
    }

    return size;
}  
