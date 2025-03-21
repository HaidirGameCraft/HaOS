#define BOOT_STAGE_DEFINE

#include <stdio.h>
#include <fs/fat.h>
#include <font.h>
#include <driver/video_driver.h>
#include <boot.h>
#include <memory.h>
#include <string.h>
#include <elf/elf.h>

extern void vesa_info_block();
extern void vesa_mode_info();

uint32_t *page_dir;
uint32_t *kernel_page_table;

BootInfo_t *bootInfo = (BootInfo_t*) 0x00020000;
void boot_stage_main()
{
    VDriver_Initialize((VesaInfoBlock*) vesa_info_block, (VesaModeInfo*) vesa_mode_info);
    VDrider_ClearScreen(0x00000000);
    fat_initialize();
    
    Font_Init("BITFONT BIN");

    printf("Hello, World!\n");

    bootInfo->font_address = 0x000A0000;
    bootInfo->vesa_info_block = (VesaInfoBlock*) vesa_info_block;
    bootInfo->vesa_mode_info = (VesaModeInfo*) vesa_mode_info;
    
    // Setup Paging
    page_dir = (uint32_t*) 0x00400000;
    uint32_t* first_page_table = (uint32_t*) 0x00401000;
    kernel_page_table = (uint32_t*) 0x00402000;
    uint32_t* heap_page_table = (uint32_t*) 0x00403000;
    uint32_t* vesa_mode_page_table = (uint32_t*) 0x00404000;
    uint32_t* page_dir_page_table = (uint32_t*) 0x00405000;
    uint32_t* kernel2_page_table = (uint32_t*) 0x00406000;

    memset(page_dir, 0, 0x1000);
    memset(first_page_table, 0, 0x1000);
    memset(kernel_page_table, 0, 0x1000);
    memset(heap_page_table, 0, 0x1000);
    memset(vesa_mode_page_table, 0, 0x1000);
    
    FILEADDR dir = find_file("KERNEL  BIN");
    //int count = read_file(&dir, (void*) 0x00100000, READ_ALL);

    uint16_t width = ((VesaModeInfo*) vesa_mode_info)->width;
    uint16_t height = ((VesaModeInfo*) vesa_mode_info)->height;
    uint8_t bpp = ((VesaModeInfo*) vesa_mode_info)->bytes_per_pixel;
    uint32_t addr = ((VesaModeInfo*) vesa_mode_info)->framebuffer;

    for(int i = 0; i < 1024; i++)
    {
        first_page_table[i] = (i * 0x1000) | 0x03;
    }

    addr_t kernel_virt_addr = 0xC0100000;
    addr_t kernel_phys_addr = 0x00100000;

    for(int i = 0; i < 1024; i++)
    {
        uint32_t virt_addr = kernel_virt_addr + i * 0x1000;
        uint32_t phys_addr = kernel_phys_addr + i * 0x1000;
        kernel_page_table[(virt_addr >> 12) & 0x3FF] = (phys_addr) | 0x03;

        uint32_t virt_addr2 = kernel_virt_addr + 1024 * 0x1000 + i * 0x1000;
        uint32_t phys_addr2 = kernel_phys_addr + 1024 * 0x1000 + i * 0x1000;
        kernel2_page_table[(virt_addr2 >> 12) & 0x3FF] = (phys_addr2) | 0x03;
    }

    for(int i = 0; i < 1024; i++)
    {
        uint32_t virt_addr = 0x00400000 + i * 0x1000;
        page_dir_page_table[(virt_addr >> 12) & 0x3FF] = (0x400000 + i * 0x1000) | 0x03;
    }


    for(int i = 0; i < 1024; i++)
    {
        uint32_t virt_addr = 0x02000000 + i * 0x1000;
        heap_page_table[(virt_addr >> 12) & 0x3FF] = (0x02000000 + i * 0x1000) | 0x03;
    }

    for(int i = 0; i < 1024; i++)
    {
        uint32_t virt_addr = ((uint32_t) addr + i * 0x1000);
        vesa_mode_page_table[i] = virt_addr | 0x03;
    }

    page_dir[0] = ((uint32_t) first_page_table) | 0x03;
    page_dir[0x00400000 >> 22] = ((uint32_t) page_dir_page_table) | 0x03;
    page_dir[0x02000000 >> 22] = ((uint32_t) heap_page_table) | 0x03;
    page_dir[kernel_virt_addr >> 22] = ((uint32_t) kernel_page_table) | 0x03;
    page_dir[addr >> 22] = ((uint32_t) vesa_mode_page_table) | 0x03;

    // Enable Paging
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_dir));
    
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
    
    printf("KERNEL  BIN: Has been found\n");
    memory_initialize();

    FILEADDR kernel_file = find_file("KERNEL  ELF");
    char* buffer_kernel = read_all_file(&kernel_file);

    ELF32_Header header = ELF_readHeaderFile(buffer_kernel);
    ELF_readFile(&header, buffer_kernel);

    free( buffer_kernel );
    
    void (*_kernel)(BootInfo_t*) = (void (*)(BootInfo_t*)) 0xC0100000;
    _kernel(bootInfo);
    __asm__ volatile("hlt");

}