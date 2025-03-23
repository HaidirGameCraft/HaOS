#ifndef DISK_DRIVER
#define DISK_DRIVER

#include <stdint.h>

#define ATA_PRIMARY_DRIVE       0x1f0
#define ATA_MASTER_BIT          0xE
#define ATA_SLAVE_BIT           0xF

#define ATA_SECTOR_COUNT_PORT   0x02
#define ATA_LBA_LOW_PORT        0x03
#define ATA_LBA_MID_PORT        0x04
#define ATA_LBA_HIGH_PORT       0x05
#define ATA_SELECT_DRIVE        0x06
#define ATA_COMMAND_PORT        0x07
#define ATA_STATUS_PORT         0x07
#define ATA_DATA_PORT           0x00

#define ATA_STATUS_BSY_BIT      1 << 7
#define ATA_STATUS_RDY_BIT      1 << 6
#define ATA_STATUS_DF_BIT       1 << 5
#define ATA_STATUS_SRV_BIT      1 << 4
#define ATA_STATUS_DRQ_BIT      1 << 3
#define ATA_STATUS_CORR_BIT     1 << 2
#define ATA_STATUS_IDX_BIT      1 << 1
#define ATA_STATUS_ERR_BIT      1 << 0

#define ATA_COMMAND_READ        0x20
#define ATA_COMMAND_WRITE       0x30

void disk_read(uint32_t lba, void* buffer);
void disk_write(uint32_t lba, void* buffer);
void disk_write_addr(uint32_t addr, void* buffer, size_t size);

#endif