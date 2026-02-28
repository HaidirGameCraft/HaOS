#include <type.h>
#include <serial.h>
#include <io.h>
#include <fat.h>
#include "string.h"
#include "config.h"

#define KERNEL_ADDRESS  0x100000
bootstage_info_t bootstage_info;

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
    vesa_information();

    bootstage_info.flags |= (1 << BOOTSTAGE_FLAGS_GRAPHICS_MODE_BIT);
    bootstage_info.width = vesaModeInfo.width;
    bootstage_info.height = vesaModeInfo.height;
    bootstage_info.bytes_per_pixel = vesaModeInfo.bpp;
    bootstage_info.framebuffer = vesaModeInfo.framebuffer;

    fat_init();

    printf("%x, %i\n", 0x1000, 1234);

    fat_filedirectory_t file = fat_findFile("kernel.bin");
    if( file.attributes != 0 )
    {
        fat_readFile( &file, (char*) KERNEL_ADDRESS );
        
        kernel_file_header_t* file_header = (kernel_file_header_t*) KERNEL_ADDRESS;
        if( strncmp( file_header->identifier, "KERNEL    ", 11) == 0 )
        {
            printf("File Identify: %s\n", file_header->identifier);
            printf("File Address Start: 0x%x\n", file_header->address_start);
            printf("File Address End: 0x%x\n", file_header->address_end);
            printf("File Entry Start: 0x%x\n", file_header->entry_start );

            void (*__entry)( bootstage_info_t* ) = (void (*)( bootstage_info_t* )) file_header->entry_start;
            printf("Address of Bootstage Info: 0x%x\n", &bootstage_info );
            __entry( &bootstage_info );
        } else {
            printf("File is not Kernel Identifier\n");
        }
    }
}