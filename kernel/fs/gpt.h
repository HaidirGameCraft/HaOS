#pragma once

#include <type.h>

typedef struct {
    uint8_t  boot_indicator;
    uint8_t  chs_start[3];
    uint8_t  os_type;
    uint8_t  chs_end[3];
    uint32_t lba_start;
    uint32_t lba_end;
} __attribute__((packed)) protective_mbr_t;

typedef struct {
    uint64_t signature;
    uint32_t revision;
    uint32_t hdr_size;
    uint32_t checksum_crc32;
    uint32_t reserved;
    uint64_t lba;
    uint64_t lba_gpt;
    uint64_t first_usable_block;
    uint64_t last_usable_block;
    uint64_t guid[2];
    uint64_t lba_start;
    uint32_t num_partition;
    uint32_t size_entries_array;
    uint32_t crc32_entry_array; 
} __attribute__((packed)) partition_table_header_t;

typedef struct {
    uint64_t type[2];
    uint64_t unique[2];
    uint64_t lba_start;
    uint64_t lba_end;
    uint64_t attribute;
    uint8_t  name[72];   
} __attribute__((packed)) partition_entry_t;

#define PARTITION_TABLE_SIZE sizeof( partition_table_header_t )
#define PARTITION_ENTRY_SIZE sizeof( partition_entry_t )

uint64_t gpt_get_lba_start_volume( int index );