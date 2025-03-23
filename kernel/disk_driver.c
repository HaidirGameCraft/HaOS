#include <driver/disk_driver.h>
#include <cpu/ports.h>
#include <memory.h>
#include <stdio.h>

void __init_drive_lba(uint32_t lba)
{
    port_outb(ATA_PRIMARY_DRIVE | ATA_SELECT_DRIVE, (ATA_MASTER_BIT << 4) | ( (lba >> 24) & 0x0F));

    port_outb(ATA_PRIMARY_DRIVE | ATA_SECTOR_COUNT_PORT, 1);
    port_outb(ATA_PRIMARY_DRIVE | ATA_LBA_LOW_PORT, lba & 0xFF);
    port_outb(ATA_PRIMARY_DRIVE | ATA_LBA_MID_PORT, (lba >> 8) & 0xFF);
    port_outb(ATA_PRIMARY_DRIVE | ATA_LBA_HIGH_PORT, (lba >> 16) & 0xFF);
}

void wait_drive_busy()
{
    while( port_inb(ATA_PRIMARY_DRIVE | ATA_STATUS_PORT) & ATA_STATUS_BSY_BIT );
}

void wait_drive_ready_accept_PIO()
{
    while( !(port_inb(ATA_PRIMARY_DRIVE | ATA_STATUS_PORT ) & ATA_STATUS_DRQ_BIT) );
}

void disk_read(uint32_t lba, void* buffer) {
    __init_drive_lba( lba );

    port_outb(ATA_PRIMARY_DRIVE | ATA_COMMAND_PORT, ATA_COMMAND_READ);         // READ SECTOR CMD

    // Wait until driver is not busy
    wait_drive_busy();

    for(int i = 0; i < 256; i++)
    {
        ((uint16_t*) buffer)[i] = port_inw(ATA_PRIMARY_DRIVE | ATA_DATA_PORT);
    }
}

void disk_write(uint32_t lba, void* buffer) {
    __init_drive_lba( lba );

    port_outb(ATA_PRIMARY_DRIVE | ATA_COMMAND_PORT, ATA_COMMAND_WRITE);         // WRITE SECTOR CMD

    wait_drive_busy();
    wait_drive_ready_accept_PIO();

    char* b = (char*) buffer;

    for(int i = 0; i < 256; i++)
    {
        port_outw(ATA_PRIMARY_DRIVE | ATA_DATA_PORT, ((uint16_t*) buffer)[i]);
    }

    // Wait until the driver has complete write
    wait_drive_busy();
}

void disk_write_addr(uint32_t addr, void* buffer, size_t size) {
    int j = 0;
    char* _b = (char*) malloc( 512 );
    while( size > 0 )
    {
        uint32_t lba = (int)(addr / 512);
        uint32_t offset = addr % 512;

        disk_read(lba, (void*) _b);
        __init_drive_lba( lba );

        for(int i = offset; i < 512; i++)
        {
            if( size == 0)
                break;

            _b[i] = ((uint8_t*) buffer)[j];
            j++;
            size--;
            addr++;
        }
    
        port_outb(ATA_PRIMARY_DRIVE | ATA_COMMAND_PORT, ATA_COMMAND_WRITE);         // READ SECTOR CMD
    
        wait_drive_busy();
        wait_drive_ready_accept_PIO();

        for(int i = 0; i < 512; i++)
        {
            port_outb(ATA_PRIMARY_DRIVE | ATA_DATA_PORT, ((uint8_t*) _b)[i]);
        }

        wait_drive_busy();
    }
    free(_b);
}