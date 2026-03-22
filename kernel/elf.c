#include <elf.h>

#include <serial.h>
#include <fs/fat.h>
#include <alloc.h>
#include <string.h>
#include <io.h>
#include <page.h>

elf64_header_t* elf64_create( const char* filename ) {
    elf64_header_t* hdr = ( elf64_header_t* ) new_alloc( sizeof( elf64_header_t ) );
    char* temp = ( char* ) new_alloc( (qword) fat_sizeSector() );


    fat_file_ptr file = fat_fileOpen( filename );
    if( file.current_cluster != 0xFFFFFFFF ) {
        // Now we read the first 512 
        printf("[Debug]: %s is found\n", filename );

        fat_read( &file, hdr, sizeof( elf64_header_t ) );

    } else {
        printf("[Debug]: %s is \"NOT\" found\n", filename );
    }

    free_alloc( temp );

    return hdr;
}

byte            elf64_verity( elf64_header_t* hdr ) {
    if( hdr->magic_number[0] != 0x7F || strncmp("ELF", &hdr->magic_number[1], 3 ) == 1  )
    {
        printf("ELF: This is not ELF file because not following their ELF format\n");
        return 1;   // NOT ELF
    }

    printf("ELF Magic Number: %x", (dword)( (dword*) hdr->magic_number )[0] );
    return 0; // Success
}

void            elf64_showDetails( elf64_header_t* hdr ) {
    printf("ELF Magic Number: %x\n", hdr->magic_number[0] );
    printf("ELF Magic Number Word: %c%c%c\n", hdr->magic_number[1], hdr->magic_number[2], hdr->magic_number[3] );
    printf("ELF Machine: %x, ELF Endian\n", hdr->machine);
    printf("ELF Header Version: %x\n", hdr->machine);

    printf("\n\n");
    printf("Program Entry: %x\n", hdr->prog_entry_off );
    printf("Program Table Offset: %x\n", hdr->prog_header_table_off);
    printf("Number of Program Tables: %i\n", hdr->count_prog);
    printf("Size of Program Table: %i\n\n", hdr->prog_header_size );

    printf("Section Table Offset: %x\n", hdr->sec_header_table_off);
    printf("Number of Section Tables: %i\n", hdr->count_sec);
    printf("Size of Section Table: %i\n\n", hdr->sec_header_size );
}

qword           elf64_load( const char* filename ) {

    elf64_header_t* hdr = ( elf64_header_t* ) new_alloc( sizeof( elf64_header_t ) );
    fat_file_ptr file = fat_fileOpen( filename );
    if( file.current_cluster == 0xFFFFFFFF ) {
        // Now we read the first 512 
        printf("[Debug]: %s is \"NOT\" found\n", filename );   
    }

    printf("[Debug]: %s is found\n", filename );
    fat_read( &file, hdr, sizeof( elf64_header_t ) );

    // Verify ELF
    if( hdr->magic_number[0] != 0x7F || strncmp("ELF", &hdr->magic_number[1], 3 ) == 1  )
    {
        printf("ELF: This is not ELF file because not following their ELF format\n");
        free_alloc( hdr );
        return 1;   // NOT ELF
    }

    elf64_showDetails( hdr );

    // Create and read program list from file
    elf64_program_t* programs = ( elf64_program_t* ) new_alloc( sizeof( elf64_program_t ) * hdr->count_prog );
    fat_seek( &file, hdr->prog_header_table_off );
    fat_read( &file, programs, sizeof( elf64_program_t ) * hdr->count_prog );

    printf("ELF Program List: \n");
    for( int i = 0; i < hdr->count_prog; i++ )
    {
        elf64_program_t* prog = &programs[i];
        printf("%i: Virt=%x, Phys=%x, FileSize=%x, Align=%x\n", i, prog->virt_addr, prog->phys_addr, prog->filesize, prog->align );
    }

    // Load code into specific address
    //qword* pmlt4_new = ( qword* ) page_create();
    //page_setPMLT4( pmlt4_new );
    for( int i = 0; i < hdr->count_prog; i++ ) {
        elf64_program_t* prog = &programs[i];
        // Mapping the virtual address
        page_mapv( prog->virt_addr, prog->memsize );

        fat_seek( &file, (qword) prog->prog_off );
        fat_read( &file, (void*) prog->virt_addr, prog->filesize );
    }

    int (*__entry)( void ) = (int (*)(void)) hdr->prog_entry_off;
    int value = __entry();

    printf("The program exit by Code (%x)\n", value);

    free_alloc( hdr );
    return 0;
}
