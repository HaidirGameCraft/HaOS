#include <io.h>
#include "fat.h"
#include <serial.h>
#include "disk.h"
#include "string.h"

fat_header_t __header;
fat_extended_boot_t __extended;
void convert_name_to_fat16name( char* filename ) {
    strupper( filename );
    dword size = strsize( filename );

    int dotFileIndex = -1;
    for( int i = 0; i < size; i++ )
    {
        if( filename[i] == '.' )
        {
            dotFileIndex = i;
            break;
        }
    }

    char fat16Name[11];
    for( int i = 0; i < 8; i++ ) {
        if( i >= dotFileIndex )
            fat16Name[i] = ' ';
        else
            fat16Name[i] = filename[i];
    }

    for( int i = 8; i < 11; i++ )
        fat16Name[i] = filename[(dotFileIndex + 1) + (i - 8)];

    memory_copy( filename, &fat16Name, 11 );
}   


void fat_init() {
    ata_read_disk( 0, 0, &__header, sizeof( fat_header_t ) );
    ata_read_disk( 0, sizeof( fat_header_t ), &__extended, sizeof( fat_extended_boot_t ) );

    dword fat_start = __header.reserved_sector;
    dword root_dirs_start = fat_start + __header.fat_number * __header.sector_per_fat;
    dword root_dir_sectors = __header.root_dir_entries / ( __header.bytes_per_sector / sizeof( fat_filedirectory_t ) );
    dword data_start = root_dirs_start + root_dir_sectors;
    printf("FAT Start: 0x%x\n", fat_start * 0x200 );
    printf("Root Dir Start: 0x%x\n", root_dirs_start );
    printf("Data Sector Start: 0x%x\n", data_start );
}

fat_filedirectory_t fat_findFile( const char* filename ) {

    dword fat_start = __header.reserved_sector;
    dword root_dirs_start = fat_start + __header.fat_number * __header.sector_per_fat;
    dword root_dir_sectors = __header.root_dir_entries / ( __header.bytes_per_sector / sizeof( fat_filedirectory_t ) );
    dword data_start = root_dirs_start + root_dir_sectors;
    
    fat_filedirectory_t file_result;
    memory_zero( &file_result, sizeof( fat_filedirectory_t ) );
    byte buffer[512];
    char fname[64];
    int index_fname = 0;
    int indexPath = 0;
    int sizeFilename = strsize( filename );
    byte indexExtFile = 0;

    while( indexPath <= sizeFilename )
    {
        if( filename[indexPath] == '/' || filename[indexPath] == 0 )
        {
            convert_name_to_fat16name( fname );
            fname[11] = 0;
            printf("Searching file \"%s\"\n", fname );
            
            for( int i = 0; i < root_dir_sectors; i++ )
            {
                // printf("Root Dir Start: 0x%x\n", (root_dirs_start + i) );
                ata_read_disk( root_dirs_start + i, 0, buffer, 512 );
                fat_filedirectory_t* dirs = ( fat_filedirectory_t* ) buffer;

                for( int j = 0; j < 16; j++ )
                {
                    
                    if( strncmp( dirs[j].name, fname, 11 ) == 0 )
                    {
                        printf("[File] %s is Found\n", fname);
                        memory_copy( &file_result, &dirs[j], sizeof( fat_filedirectory_t ) );
                        return file_result;
                    }
                }
            }
            

            indexExtFile = 0;
            index_fname = 0;
            fname[ index_fname ] = 0;
            indexPath++;
            continue;
        }

        if( filename[indexPath] == '.' ) indexExtFile = index_fname;
        fname[ index_fname++ ] = filename[indexPath];
        fname[ index_fname ] = 0;
        indexPath++;
    }

    printf("[File] %s is not found\n", filename );

    return file_result;
}

void fat_readFile( const fat_filedirectory_t* file, char* buffer ) {
    dword fat_start = __header.reserved_sector;
    dword root_dirs_start = fat_start + __header.fat_number * __header.sector_per_fat;
    dword root_dir_sectors = __header.root_dir_entries / ( __header.bytes_per_sector / sizeof( fat_filedirectory_t ) );
    dword data_start = root_dirs_start + root_dir_sectors;
    byte __buffer[512];

    dword cluster = file->high_cluster << 16 | file->low_cluster;
    while( cluster < FAT16_NOCLUSTER )
    {
        for( int i = 0; i < __header.sector_per_cluster; i++ )
        {
            ata_read_disk( data_start + ( cluster - 2 ) * __header.sector_per_cluster + i, 0, buffer, 512 );
            //printf("Buffer Address: 0x%x, Sector LBA: %i\n", buffer, data_start + ( cluster - 2 ) * __header.sector_per_cluster + i);
            buffer = buffer + 512;
        }
        
        dword segFat = (dword)( cluster / 256 ); // bytes_per_sectors / size_of_word = 256
        dword offFat = (dword)( cluster % 256 );

        ata_read_disk( fat_start + segFat, 0, __buffer, 512 );
        cluster = ((word*) __buffer)[ offFat ];
    }
}