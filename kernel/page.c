#include <page.h>

#define __SYSCALL__FUNCTION__
#include <syscall.h>

extern void _kernel_start();
extern void _kernel_end();

uint32_t *page_directory = (uint32_t*) 0x00400000;
uint8_t bitmap_physical_page[1024 * 1024];

addr_t Page_AllocNewPage()
{
    int length = (0x00500000 - 0x00400000 - 0x1000) / 0x1000;
    int i = 0;
    while( i < length )
    {
        uint32_t* page_table = (uint32_t*)(0x00401000 + i * 0x1000);
        if( !(page_table[i] & ATTR_PAGE_PRESENT) )
        {
            return (addr_t) page_table;
        }

        i++;
    }

    return 0;
}

addr_t Page_GetPhysicalAvailable()
{
    for(int i = 0; i < 1024 * 1024; i++)
    {
        if( bitmap_physical_page[i] == 0)
        {
            bitmap_physical_page[i] = 1;
            return i * 0x1000;
        }
    }
}

void Page_FreePhysical(uint16_t index) {
    if( bitmap_physical_page[index] == 1)
        bitmap_physical_page[index] = 0;
}

void Page_Initialize() {
    for(int i = 0; i < 1024; i++)
    {
        if( page_directory[i] & ATTR_PAGE_PRESENT )
        {
            uint32_t* page_table = (uint32_t*)(page_directory[i] & ~0x3FF);
            for(int j = 0; j < 1024; j++)
            {
                if( page_table[j] & ATTR_PAGE_PRESENT )
                {
                    uint32_t phys_addr = page_table[j] & ~0x3FF;
                    uint32_t index = phys_addr / 0x1000;
                    bitmap_physical_page[index] = 1;
                }
            }
        }
    }
}

void Page_SetMapping(addr_t virtual_address, uint16_t attribute, size_t size) {
    int length = (int)(size / PAGE_SIZE) + 1;

    for(int off = 0; off < length; off++)
    {
        uint32_t pd_index = (virtual_address + off * PAGE_SIZE) >> 22;
        uint32_t pt_index = (virtual_address + off * PAGE_SIZE) >> 12 & 0x3FF;

        addr_t physical = Page_GetPhysicalAvailable();
        if( !(page_directory[pd_index] & ATTR_PAGE_PRESENT) )
        {
            uint32_t alloc_page = Page_AllocNewPage();
            if( alloc_page != 0 )
            {
                ((uint32_t*) alloc_page)[pt_index] = physical | attribute;
                page_directory[pd_index] = alloc_page | ATTR_PAGE_PRESENT | ATTR_PAGE_RW;
            }
        }
        else {
            ((uint32_t*) (page_directory[pd_index] & ~0x3FF))[pt_index] = physical | attribute;
        }
    }
}

void Page_ResetMapping(addr_t virtual_address, size_t size) {
    int length = (int)(size / PAGE_SIZE) + 1;

    for(int off = 0; off < length; off++)
    {
        uint32_t pd_index = (virtual_address + off * PAGE_SIZE) >> 22;
        uint32_t pt_index = (virtual_address + off * PAGE_SIZE) >> 12 & 0x3FF;

        // Page_FreePhysical(pd_index + pt_index * 1024);
        // page_t* entry = &ptable.entries[pd_index];
        // entry->entries[pt_index] = (pd_index + pt_index * 1024) * PAGE_SIZE | ATTR_PAGE_PRESENT | ATTR_PAGE_RW;
    }
}

VOID   sys_mmap(DWORD __virtual_address, DWORD __size, BYTE __flags) {
    Page_SetMapping(__virtual_address, __flags, __size);
}