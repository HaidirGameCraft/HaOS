#pragma once

#include <type.h>

#define ATA_PRIMARY_BUS     0x1F0
#define ATA_SECONDARY_BUS   0x170
#define ATAPIO_COMMAND      7
#define ATAPIO_STATUS       7
#define ATAPIO_DATA         0


// Read data sector from disk
void ata_read_disk( dword lba, dword offset, void* buffer, size_t size );

// Write data sector to disk
void ata_write_disk( dword lba, dword offset, void* buffer, size_t size);