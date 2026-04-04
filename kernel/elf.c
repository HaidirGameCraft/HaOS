#include <elf.h>

#include <process/users.h>
#include <serial.h>
#include <fs/fat.h>
#include <alloc.h>
#include <string.h>
#include <io.h>
#include <page.h>
#include <cpu/syscall.h>
#include <process/task.h>

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

fat_file_ptr*   elf64_loadLibrary( const char* filename ) {
    elf64_header_t* hdr = ( elf64_header_t* ) new_alloc( sizeof( elf64_header_t ) );
//    printf("[ELF]: %x", *((dword*) &hdr->magic_number[0]));

    fat_file_ptr* fptr = open( filename );
    if( fptr->current_cluster == 0xFFFFFFFF ) {
        printf("[Error Load Library]: %s is not found\n", filename );
        close( fptr );
        free_alloc( hdr );
        return NULL;
    }

//    printf("[Library] Current Cluster: %x\n", fptr->current_cluster);

    read( fptr, (char*) hdr, sizeof( elf64_header_t ) );
//    printf("HDR Address: %x\n", (qword) hdr );
    if( hdr->magic_number[0] != 0x7F || strncmp( &hdr->magic_number[1], "ELF", 3 ) != 0 )
    {
        printf("[Error Load Library]: %s is not ELF file\n", filename );
        close( fptr );
        free_alloc( hdr );
        return NULL;
    }

    elf64_program_t* programs = ( elf64_program_t* ) new_alloc( hdr->prog_header_size * hdr->count_prog );
    fat_seek( fptr, hdr->prog_header_table_off );
    read( fptr, programs, hdr->prog_header_size * hdr->count_prog );

    // Load to memory
    for( int i = 0; i < hdr->count_prog; i++ )
    {
        elf64_program_t* prog = ( elf64_program_t* ) &programs[i];
        if( prog->type_segment != EPT_LOAD )
            continue;

        page_mapv( prog->virt_addr, prog->memsize, PAGE_PRESENT | PAGE_READWRITE | PAGE_USERSPACE );
        memzero((void*)prog->virt_addr, prog->memsize );

        fat_seek( fptr, prog->prog_off );
        read( fptr, (char*) prog->virt_addr, prog->filesize );
    }

    free_alloc( programs );
    free_alloc( hdr );
    return fptr;
}

qword           elf64_load( const char* filename ) {

    elf64_header_t* hdr = ( elf64_header_t* ) new_alloc( sizeof( elf64_header_t ) );
    fat_file_ptr* file = open( filename );
    if( file->current_cluster == 0xFFFFFFFF ) {
        // Now we read the first 512 
        printf("%s is \"NOT\" found\n", filename );
        close( file );
        task_exit( 1 );
        return 1;
    }

    printf("[Debug]: %s is found\n", filename );
    read( file, (char*)hdr, sizeof( elf64_header_t ) );
    printf("Hel\n");

    // Verify ELF
    if( hdr->magic_number[0] != 0x7F || strncmp("ELF", &hdr->magic_number[1], 3 ) == 1  )
    {
        printf("ELF Wrong Format: (Magic Number) %x '%c%c%c'\n", hdr->magic_number[0], hdr->magic_number[1], hdr->magic_number[2], hdr->magic_number[3]);
        printf("ELF: This is not ELF file because not following their ELF format\n");
        free_alloc( hdr );
        close( file );
        task_exit( 2 );
        return 2;   // NOT ELF
    }

    //elf64_showDetails( hdr );

    // Create and read program list from file
    elf64_program_t* programs = ( elf64_program_t* ) new_alloc( sizeof( elf64_program_t ) * hdr->count_prog );
    fat_seek( file, hdr->prog_header_table_off );
    read( file, programs, sizeof( elf64_program_t ) * hdr->count_prog );
    
    //printf("ELF Program List: \n");
    for( int i = 0; i < hdr->count_prog; i++ )
    {
        elf64_program_t* prog = &programs[i];
        printf("%i: Virt=%x, Phys=%x, FileSize=%x, Align=%x\n", i, prog->virt_addr, prog->phys_addr, prog->filesize, prog->align );
    }

    // Load code into specific address
    //qword* pmlt4_new = ( qword* ) page_create();
    //page_setPMLT4( (qword) pmlt4_new );
    //page_enable( (qword) pmlt4_new );
    for( int i = 0; i < hdr->count_prog; i++ ) {
        elf64_program_t* prog = &programs[i];
        if( prog->type_segment != EPT_LOAD )
            continue;
        // Mapping the virtual address
        page_mapv( prog->virt_addr, prog->memsize,PAGE_PRESENT | PAGE_READWRITE );
        memzero( (void*) prog->virt_addr, prog->memsize );

        fat_seek( file, (qword) prog->prog_off );
        read( file, (void*) prog->virt_addr, prog->filesize );
    }

    // load requirement program
    printf("HDR Address: %x\n", (qword) hdr );
//    fat_file_ptr* fptr_libc = elf64_loadLibrary("libc.so");

    int (*__entry)( void ) = (int (*)(void)) hdr->prog_entry_off;
    int value = __entry();
    // task_create( (qword) hdr->prog_entry_off );
    //qword user_stack = page_alloc4K(PAGE_READWRITE | PAGE_PRESENT );
    //turn_to_userspace((qword) __entry, user_stack);

    //page_installMainPage();
    //
   // page_umapv(user_stack, 0x1000);


    // printf("The program exit by Code (%x)\n", value);

    // Unmapped Virtual address
    for( int i = 0; i < hdr->count_prog; i++ ) {
        if( programs[i].type_segment != EPT_LOAD )
            continue;
        page_umapv(programs[i].virt_addr, programs[i].memsize);
    }

//    close( fptr_libc );

    printf("[Debug elf64_load]: Deleting ELF64 Programs Lists\n");
    free_alloc( programs );
    printf("[Debug elf64_load]: Deleting ELF64 Header\n");
    free_alloc( hdr );
    printf("[Debug elf64_load]: Close file\n");
    close( file );
    printf("[Debug elf64_load]: Clear done\n");
    task_exit( 0 );
    return 0;
}
