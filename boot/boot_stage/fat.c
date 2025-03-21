#define BOOT_STAGE_DEFINE

#include <fs/fat.h>
#include <driver/disk_driver.h>
#include <driver/vga_driver.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

struct FAT_Header fat_header;
uint32_t fat_start_sector = 0;
uint32_t fat_data_sector = 0;
char buffer[512] = {0};

void fat_initialize() {
    disk_read(0, buffer);
    memcpy(&fat_header, buffer, sizeof( struct FAT_Header ));

    fat_start_sector = fat_header.reserved_sectors;
    fat_data_sector = fat_start_sector + fat_header.fat_numbers * fat_header.sectors_per_fat;
}

struct FAT_Header* get_fat_header() {
    return &fat_header;
}

FILEADDR find_file(const char* filename) {
    FILEADDR f;
    struct FATDirectory _target;
    memset(&_target, 0, sizeof( struct FATDirectory ));
    uint32_t cluster = 2;
    size_t directories_per_sector = fat_header.bytes_Per_sector / sizeof( struct FATDirectory );
    
    while( cluster < 0x0FFFFFFF )
    {
        uint32_t cluster_region = fat_data_sector + (cluster - 2) * fat_header.sectors_per_cluster;
        disk_read(cluster_region, buffer);
        
        struct FATDirectory *dirs = (struct FATDirectory*) buffer;
        for(int i = 0; i < 16; i++)
        {
            char* dname = dirs[i].filename;
            if( strncmp(filename, dirs[i].filename, 11 ) == 0 )
            {
                memcpy( &_target, &dirs[i], sizeof( struct FATDirectory ));
                uint32_t file_local_addr = cluster_region * 512 + i * sizeof(struct FATDirectory);
                f.addr = (fat_data_sector + ( ((_target.high_cluster << 16 ) | _target.low_cluster) - 2 ) * fat_header.sectors_per_cluster) * 512;
                f.file_addr = cluster_region * 512 + i * sizeof( struct FATDirectory );
                
                return f;
            }
        }
        
        int FAT_Seg = (int)( cluster / 128 );
        int FAT_Off = cluster % 128;
        disk_read(fat_start_sector + FAT_Seg, buffer);
        cluster = ((uint32_t*) buffer)[FAT_Off];
    }

    return f;
}

uint32_t read_file(FILEADDR* file, void* _dest, uint32_t type) {

    // buffer
    disk_read((int)(file->file_addr / 512), buffer);
    struct FATDirectory dir = ((struct FATDirectory*) buffer)[ (int)((file->file_addr % 512) / sizeof( struct FATDirectory )) ];
    
    //f.segment = (fat_data_sector + ( ((_target.high_cluster << 16 ) | _target.low_cluster) - 2 ) * fat_header.sectors_per_cluster) * 512
    //fds + ( cluster - 2 ) * sectors = segment / 512
    // cluster = ( (segment / 512) - fds ) / sectors + 2
    uint32_t cluster = ( (file->addr / 512) - fat_data_sector ) / fat_header.sectors_per_cluster + 2;
    //int cr = fat_data_sector + ( cluster - 2 ) * fat_header.sector
    size_t filesize = dir.file_size;
    uint32_t count = 0;

    if( type == ONE_BY_ONE )
    {
        if( cluster < 0x0FFFFFFF )
        {
            int cluster_region = file->addr / 512;

            disk_read(cluster_region, _dest);
            _dest += fat_header.bytes_Per_sector;
            count += fat_header.bytes_Per_sector;
            
            int FAT_Seg = (int)( cluster / 128 );
            int FAT_Off = cluster % 128;
            disk_read(fat_start_sector + FAT_Seg, buffer);
            cluster = ((uint32_t*) buffer)[FAT_Off];
            file->addr = (fat_data_sector + (cluster - 2 ) * fat_header.sectors_per_cluster) * 512;
        }
    }
    else {
        while( cluster < 0x0FFFFFFF )
        {
            int cluster_region = file->addr / 512;

            disk_read(cluster_region, _dest);
            _dest += fat_header.bytes_Per_sector;
            count += fat_header.bytes_Per_sector;
            
            int FAT_Seg = (int)( cluster / 128 );
            int FAT_Off = cluster % 128;
            disk_read(fat_start_sector + FAT_Seg, buffer);
            cluster = ((uint32_t*) buffer)[FAT_Off];
            file->addr = (fat_data_sector + (cluster - 2 ) * fat_header.sectors_per_cluster) * 512;
        }
    }
    

    return count;
}

char* read_all_file(FILEADDR* file) {
    disk_read((int)(file->file_addr / 512), buffer);
    struct FATDirectory dir = ((struct FATDirectory*) buffer)[ (int)((file->file_addr % 512) / sizeof( struct FATDirectory )) ];
    
    uint32_t cluster = (dir.high_cluster << 16) | dir.low_cluster;
    size_t filesize = dir.file_size;

    char* _dest = (char*) malloc( filesize );
    char* backup = _dest;

    while( cluster < 0x0FFFFFFF )
    {
        int cluster_region = fat_data_sector + (cluster - 2 ) * fat_header.sectors_per_cluster;

        if( filesize >= 512 ) {
            disk_read(cluster_region, _dest);
            _dest += fat_header.bytes_Per_sector;
            filesize -= 512;
        } else {
            disk_read(cluster_region, buffer);
            memcpy(_dest, buffer, filesize);
            filesize = 0;
        }
        
        int FAT_Seg = (int)( cluster / 128 );
        int FAT_Off = cluster % 128;
        disk_read(fat_start_sector + FAT_Seg, buffer);
        cluster = ((uint32_t*) buffer)[FAT_Off];
    }
    return backup;
}