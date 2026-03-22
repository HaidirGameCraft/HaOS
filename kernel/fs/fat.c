#include <io.h>
#include "fat.h"
#include <serial.h>
#include "disk.h"
#include <string.h>
#include <page.h>

#include <alloc.h>

fat_header_t __header;
fat_extended_boot_t __extended;
dword fat_getNextCluster( dword cluster );
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

    memcopy( filename, &fat16Name, 11 );
}   


void fat_init() {
    ata_read_disk( 0, 0, &__header, sizeof( fat_header_t ) );
    ata_read_disk( 0, sizeof( fat_header_t ), &__extended, sizeof( fat_extended_boot_t ) );

    qword fat_start = __header.reserved_sector;
    dword root_dirs_start = fat_start + __header.fat_number * __header.sector_per_fat;
    dword root_dir_sectors = __header.root_dir_entries / ( __header.bytes_per_sector / sizeof( fat_filedirectory_t ) );
    dword data_start = root_dirs_start + root_dir_sectors;
    serial_printf("FAT Start: 0x%x\n", fat_start * 0x200 );
    serial_printf("Root Dir Start: 0x%x\n", root_dirs_start );
    serial_printf("Data Sector Start: 0x%x\n", data_start );
}

fat_filedirectory_t fat_findFile( const char* filename ) {

    dword fat_start = __header.reserved_sector;
    dword root_dirs_start = fat_start + __header.fat_number * __header.sector_per_fat;
    dword root_dir_sectors = __header.root_dir_entries / ( __header.bytes_per_sector / sizeof( fat_filedirectory_t ) );
    dword data_start = root_dirs_start + root_dir_sectors;
    
    fat_filedirectory_t file_result;
    memzero( &file_result, sizeof( fat_filedirectory_t ) );
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
                        memcopy( &file_result, &dirs[j], sizeof( fat_filedirectory_t ) );
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

dword fat_sizeSector() {
    return __header.sector_per_cluster * __header.bytes_per_sector;
}

dword fat_getNextCluster( dword cluster ) {
    char buffer[512];
    dword seg = (dword)( cluster / 256 );
    dword off = (dword)( cluster % 256 );

    dword fat_segment = __header.reserved_sector;
    ata_read_disk( fat_segment + seg, 0, buffer, 512 );
    return ((word*) buffer)[ off ];
}

fat_file_ptr fat_fileOpen( const char* filename ) {
    fat_file_ptr fptr;
    fat_filedirectory_t fdir = fat_findFile( filename );

    if( !(fdir.attributes & FAT_DIRECTORY) )
    {
        fptr.current_cluster = fdir.low_cluster | ( fdir.high_cluster << 16 );
        fptr.next_cluster = fptr.current_cluster;

        fptr.cluster = fptr.current_cluster;
        fptr.offset = 0;
    } else {
        // Set it as 0xFFFFFFFF as File Not Found
        fptr.current_cluster = 0xFFFFFFFF;
        fptr.next_cluster = 0xFFFFFFFF;

        fptr.cluster = fptr.current_cluster;
        fptr.offset = 0;
    }
    return fptr;
}

void fat_seek( fat_file_ptr* fptr, qword pos ) {
    dword size_sector = fat_sizeSector();

    qword __len = ( qword )( pos / size_sector );
    word  __off = ( word )( pos % size_sector );  // Would be save on fptr.off

    // Find the specific Cluster from beggining cluster
    dword clus = fptr->current_cluster;
    dword fat_start = __header.reserved_sector;
    word* fat_buffer = ( word* ) new_alloc( __header.bytes_per_sector );
    while( __len > 0 )
    {

        if( clus >= FAT16_BADCLUSTER )
        {
            printf("Error to read: FAT16_BADCLUSTER\n");
            return;
        }

        fptr->cluster = clus;   // Current cluster

        // read File Allocator Table and get next cluster
        dword __f = (dword)( clus / 256 );  // Location of FAT in file
        dword __o = (dword)( clus % 256 );  // Offset of Cluster

        ata_read_disk( fat_start + __f, 0, fat_buffer, 512 );
        clus = fat_buffer[__o];
        __len--;
    }
    free_alloc( fat_buffer );

    fptr->offset = __off;
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
            // printf("Buffer Address: 0x%x, Sector LBA: %i\n", buffer, data_start + ( cluster - 2 ) * __header.sector_per_cluster + i);
            buffer = buffer + 512;
        }
        
        cluster = fat_getNextCluster( cluster );
    }
}

void fat_read( fat_file_ptr* fptr, char* buffer, size_t size ) {
    dword fat_start = __header.reserved_sector;
    dword root_dirs_start = fat_start + __header.fat_number * __header.sector_per_fat;
    dword root_dir_sectors = __header.root_dir_entries / ( __header.bytes_per_sector / sizeof( fat_filedirectory_t ) );
    dword data_start = root_dirs_start + root_dir_sectors;

    dword size_sector = fat_sizeSector();
    dword __len = ( dword )( size / size_sector ) + ( dword )( size % size_sector > 0 );
    dword cluster = fptr->cluster;
    char* _buf = ( char* ) new_alloc( size_sector );


    for( int i = 0; i < __len; i++ )
    {

        // read data
        for( int j = 0; j < __header.sector_per_cluster; j++ )
            ata_read_disk( data_start + ( cluster - 2 ) * __header.sector_per_cluster + j, 0, (qword) _buf + j * __header.bytes_per_sector, __header.bytes_per_sector );


        if( i == __len - 1 )
        {

            for( int j = 0; j < size; j++ )
            {
                *buffer = _buf[fptr->offset + j];
                buffer = buffer + 1;
            }
            fptr->offset += size; // increase
            break;
        } else {
            
            for( int j = fptr->offset; j < size_sector; j++ )
            {
                if( size == 0 )
                    break;
                *buffer = _buf[j];
                buffer = buffer + 1;
                size = size - 1;
                fptr->offset++;
            }

            fptr->offset = fptr->offset % size_sector;
        }

        fptr->next_cluster = fat_getNextCluster( fptr->next_cluster );
        cluster = fptr->next_cluster;
        fptr->cluster = fptr->next_cluster;
    }

    free_alloc( _buf );
}

void fat_readSector( fat_file_ptr* fptr, char* buffer ) {
    dword fat_start = __header.reserved_sector;
    dword root_dirs_start = fat_start + __header.fat_number * __header.sector_per_fat;
    dword root_dir_sectors = __header.root_dir_entries / ( __header.bytes_per_sector / sizeof( fat_filedirectory_t ) );
    dword data_start = root_dirs_start + root_dir_sectors;
    byte __buffer[512];

    if( fptr->next_cluster < FAT16_NOCLUSTER )
    {
        for( int i = 0; i < __header.sector_per_cluster; i++ )
        {
            ata_read_disk( data_start + ( fptr->next_cluster - 2 ) * __header.sector_per_cluster + i, 0, buffer, 512 );
            buffer += 512;
        }

        fptr->next_cluster = fat_getNextCluster( fptr->next_cluster );
    }
}

void fat_readFile_map( const fat_filedirectory_t* file, char* buffer, dword virt ) {
    dword fat_start = __header.reserved_sector;
    dword root_dirs_start = fat_start + __header.fat_number * __header.sector_per_fat;
    dword root_dir_sectors = __header.root_dir_entries / ( __header.bytes_per_sector / sizeof( fat_filedirectory_t ) );
    dword data_start = root_dirs_start + root_dir_sectors;
    byte __buffer[512];

    dword cluster = file->high_cluster << 16 | file->low_cluster;
    int need_page_idx = 0;
    while( cluster < FAT16_NOCLUSTER )
    {
        for( int i = 0; i < __header.sector_per_cluster; i++ )
        {
            // printf("Copy the data from disk to 0x%x\n", (void*) virt );
            ata_read_disk( data_start + ( cluster - 2 ) * __header.sector_per_cluster + i, 0, (void*) virt, 512 );
            //printf("Buffer Address: 0x%x, Sector LBA: %i\n", buffer, data_start + ( cluster - 2 ) * __header.sector_per_cluster + i);
            buffer = buffer + 512;
            virt = virt + 512;
            need_page_idx++;
        }
        
        cluster = fat_getNextCluster( cluster );
    }
}
