#include "gpt.h"
#include "disk.h"
#include <alloc.h>
#include <string.h>

static partition_table_header_t partiton_table;
static uint8_t is_gpt_initialize = 0;
void gpt_initialize() {
    // We need to read partition Table on LBA 1
    char* pt_buffer = ( char* ) new_alloc( 512 );
    ata_read_disk( 1, 0, pt_buffer, 512 );
    memcopy( &partiton_table, pt_buffer, PARTITION_TABLE_SIZE );
    free_alloc( pt_buffer );
    is_gpt_initialize = 1;
}

uint64_t gpt_get_lba_start_volume( int index ) {
    if( is_gpt_initialize == 0 )
        gpt_initialize();

    uint64_t lba_partition_entry = 2;
    uint64_t sector_per_entry = 512 / PARTITION_ENTRY_SIZE;
    uint64_t lba_part = (uint64_t)( index / 4 );
    uint32_t offset = (uint32_t)( index % 4 );

    char* __tmp = ( char* ) new_alloc( 512 );
    ata_read_disk( lba_partition_entry + lba_part, 0, __tmp, 512 );
    return ((partition_entry_t*) __tmp)[offset].lba_start;
}