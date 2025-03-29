#define BOOT_STAGE_DEFINE

#define PML4T_INDEX(x) (x) >> 39 & 0x1F
#define PDP_INDEX(x) (x) >> 30 & 0x1FF
#define PD_INDEX(x) (x) >> 21 & 0x1FF
#define PT_INDEX(x) (x) >> 12 & 0x1FF

#define PAGE_PRESENT (1 << 0)
#define PAGE_RW (1 << 1)
#define PAGE_SIZE (1 << 7)

#define PAGE_ENTRIES 512
#define PAGE_4K 0x1000
#define PAGE_2MiB 0x1000 * 512
#define PAGE_4GiB 0x1000 * (512 * 512)

#include <stdio.h>
#include <fs/fat.h>
#include <font.h>
#include <driver/video_driver.h>
#include <boot.h>
#include <memory.h>
#include <string.h>
#include <elf/elf.h>
#include <page.h>

extern void vesa_info_block();
extern void vesa_mode_info();

extern void Halt();
extern void Check_CPUID();
extern void Check_LongMode();
extern void Enable_Paging(uint32_t page_dir);
extern void Go_To_LongMode();

uint32_t *page_dir;
uint32_t *kernel_page_table;

BootInfo_t *bootInfo = (BootInfo_t*) 0x00020000;
void boot_stage_main()
{
    VDriver_Initialize((VesaInfoBlock*) vesa_info_block, (VesaModeInfo*) vesa_mode_info);
    uint64_t* pml4t = (uint64_t*) 0x400000;
    uint64_t* pdp = (uint64_t*) 0x401000;
    uint64_t* pd = (uint64_t*) 0x402000;
    
    uint64_t* pt_video = (uint64_t*) 0x405000;
    uint64_t* pd_video = (uint64_t*) 0x406000;

    uint64_t* pt_heap = (uint64_t*) 0x409000;

    uint64_t addr_frame = (uint64_t) ((VesaModeInfo*) vesa_mode_info)->framebuffer;

    // heap
    for(int i = 0; i < 512; i++)
    {
        uint64_t v = ((uint64_t) 0x02000000 + i * 0x1000);
        pt_heap[i] = v | 0b11;
    }
    // memset(pd_third, 0, 0x1000);
    for(int i = 0; i < 512; i++)
    {
        uint64_t virt = ((uint64_t) addr_frame + i * 0x1000);
        pt_video[i] = ((uint64_t) virt) | 0b11;
    }

    pd_video[PD_INDEX((uint64_t) addr_frame)] = ((uint64_t) pt_video) | 0b11;
    pd[PD_INDEX((uint64_t) 0x02000000)] = ((uint64_t) pt_heap) | 0b11;
    pdp[PDP_INDEX((uint64_t) addr_frame)] = ((uint64_t) pd_video) | 0b11;
    
    // pd_third[PD_INDEX(addr_frame)] = addr_frame | PAGE_SIZE | PAGE_RW | PAGE_PRESENT;
    // pdp[PDP_INDEX(0xC0000000)] = ((uint64_t) pd_third) | 0b11;
    
    char hex[] = "0123456789ABCDEF";
    VDrider_ClearScreen(0x000000FF);
    
    fat_initialize();
    
    Font_Init("BITFONT BIN");
    
    printf("Hello, World!\n");
    
    bootInfo->font_address = 0x000A0000;
    bootInfo->vesa_info_block = (VesaInfoBlock*) vesa_info_block;
    bootInfo->vesa_mode_info = (VesaModeInfo*) vesa_mode_info;
    
    memory_initialize();
    printf("KERNEL  BIN: Has been found\n");
    
    
    FILEADDR kernel_file = find_file("KERNEL  ELF");
    char* buffer_kernel = read_all_file(&kernel_file);
    
    ELF32_Header header = ELF_readHeaderFile(buffer_kernel);
    ELF_readFile(&header, buffer_kernel);
    
    free( buffer_kernel );
    
    void (*_kernel)(BootInfo_t*) = (void (*)(BootInfo_t*)) 0xC0100000;
    _kernel(bootInfo);
    __asm__ volatile("hlt");
}
