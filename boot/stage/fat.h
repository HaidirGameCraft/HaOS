#pragma once

#include <type.h>

#define FAT_BOOTCODE_SIGNATURE      0x55AA
#define FAT_READONLY                0x01
#define FAT_HIDDEN                  0x02
#define FAT_SYSTEM                  0x04
#define FAT_VOLUME_ID               0x08
#define FAT_DIRECTORY               0x10
#define FAT_ARCHIVE                 0X20
#define FAT_LONGFILENAME            0x0F

#define FAT16_NOCLUSTER             0xFFF8
#define FAT16_BADCLUSTER            0xFFF7

#define FAT12                       0x12
#define FAT16                       0x16
#define FAT32                       0x32

typedef struct {
    byte  jmp_code[3];
    byte  oem_identifier[8];
    word bytes_per_sector;
    byte  sector_per_cluster;
    word reserved_sector;
    byte  fat_number;
    word root_dir_entries;
    word total_sector;
    byte  media_type;
    word sector_per_fat;
    word sector_per_track;
    word heads;
    dword hidden_sectors;
    dword large_sectors;
} __attribute__((packed)) fat_header_t;

typedef struct {
    byte  drive_number;
    byte  reserved;
    byte  signature;
    dword volume_id;
    byte  volume_label[11];
    byte  system_identifier[8];
    // byte  bootcode[448];
    // word part_signature;
} __attribute__((packed)) fat_extended_boot_t;

typedef struct {
    char name[11];
    byte attributes;
    byte reserved;
    byte creation_time_ms;
    word creation_time;
    word creation_date;
    word last_accessed_date;
    word high_cluster;
    word last_modify_time;
    word last_modify_date;
    word low_cluster;
    dword size;
} __attribute__((packed)) fat_filedirectory_t;

void fat_init();
fat_filedirectory_t fat_findFile( const char* filename );
void fat_readFile( const fat_filedirectory_t* file, char* buffer );