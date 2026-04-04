#include <type.h>
#include <serial.h>
#include <io.h>
#include "fat.h"
#include <font.h>
#include "string.h"
#include <page.h>
#include "cpu.h"
#include "config.h"
#include <config.h>
#define BOOTSTAGE64
#include <driver/video_driver.h>

#define KERNEL_ADDRESS  0x100000
bootstage_info_t bootstage_info;

void load_font();

void vesa_information() {
    print("Signature Vesa: ");
    for( int i = 0; i < 4; i++ )
        printf("%c", vesaInfo.signature[i]);
    printf("\n");

    printf("Width: %i, Height: %i\n", vesaModeInfo.width, vesaModeInfo.height);
    printf("Vesa Framebuffer: 0x%x\n", vesaModeInfo.framebuffer);
    printf("Char Width: %i, Char Height: %i\n", vesaModeInfo.widthChar, vesaModeInfo.heightChar );
}

void main( void ) {
    print("[OS]: Hello, COM1 Port\n");
    idt_init();
    vesa_information();
    // printf("Hai, %s\n", "Hello");
    page_init();
    //page_showList();
    
    bootstage_info.flags |= (1 << BOOTSTAGE_FLAGS_GRAPHICS_MODE_BIT);
    bootstage_info.width = vesaModeInfo.width;
    bootstage_info.height = vesaModeInfo.height;
    bootstage_info.bytes_per_pixel = vesaModeInfo.bpp;
    bootstage_info.framebuffer = vesaModeInfo.framebuffer;
 
    video_driver_init( &bootstage_info);
    video_driver_mapped();

    video_driver_clearScreen( 0x00FF00FF );

    return;
    fat_init();
    
    printf("%x, %i\n", 0x1000, 1234);
   
    // Load font
    load_font();

    fat_filedirectory_t file = fat_findFile("kernel.bin");
    fat_file_ptr kernel_fptr = fat_fileOpen( "kernel.bin" );
    
        // return;
    if( file.attributes != 0 )
    {

        // Read first Header
        qword kernel_virt_addr = 0xFFFFFFFF80100000;
        qword kernel_phys_addr = 0x00100000;
        page_mapvp( kernel_virt_addr, kernel_phys_addr, 0x1000, PAGE_PRESENT | PAGE_READWRITE );
        fat_readSector( &kernel_fptr, (char*) kernel_virt_addr );

        printf("Virtual Kernel: %x\n", kernel_virt_addr);
        
        kernel_file_header_t* file_header = (kernel_file_header_t*) kernel_virt_addr;
        if( strncmp( file_header->identifier, "KERNEL    ", 11) == 0 )
        {
            
            dword size_kernel = file_header->address_end - file_header->address_start;
            // page_mapvp( kernel_virt_addr, kernel_phys_addr, size_kernel );
            printf("Size of Kernel: 0x%x\n", size_kernel );
            page_mapvp( kernel_virt_addr, kernel_phys_addr, size_kernel, PAGE_PRESENT | PAGE_READWRITE );
            // page_showList();
            // // fat_readFile_map( &file, NULL, kernel_virt_addr );
            fat_readFile( &file, (char*) kernel_virt_addr );

            printf("File Identify: %s\n", file_header->identifier);
            printf("File Address Start: 0x%x\n", file_header->address_start);
            printf("File Address End: 0x%x\n", file_header->address_end);
            
            void (*__entry)( bootstage_info_t* ) = (void (*)( bootstage_info_t* )) file_header->entry_start;
            printf("Address of Bootstage Info: 0x%x\n", &bootstage_info );

            // printf("FrameBuffer: %x\n", bootstage_info.framebuffer);


            printf("File Entry Start: 0x%x\n", __entry );
//            __entry( &bootstage_info );
        } else {
            printf("File is not Kernel Identifier\n");
        }
    }
}

void load_font() {
    fat_filedirectory_t file = fat_findFile("font.map");
    fat_file_ptr fptr = fat_fileOpen("font.map");

    if( file.attributes & FAT_DIRECTORY || file.attributes == 0 )
    {
        printf("[Error]: font.map is not found\n");
        return;
    }

    page_mapv( DEFAULT_FONT_ADDRESS, 0x1000, PAGE_PRESENT | PAGE_READWRITE);
    fat_readSector( &fptr, (char*)  DEFAULT_FONT_ADDRESS);

    font_bitmap_header_t* hdr = ( font_bitmap_header_t* ) DEFAULT_FONT_ADDRESS;
    dword size = hdr->height_char * hdr->count_char + sizeof( font_bitmap_header_t );
    page_mapv( DEFAULT_FONT_ADDRESS, size, PAGE_PRESENT | PAGE_READWRITE);
    fat_readFile(&file, (char*) DEFAULT_FONT_ADDRESS );
    return;
}
