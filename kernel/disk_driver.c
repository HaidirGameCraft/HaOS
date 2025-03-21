#include <driver/disk_driver.h>
#include <cpu/ports.h>
#include <memory.h>
#include <stdio.h>

void disk_read(uint32_t lba, void* buffer) {
    port_outb(0x1F6, 0xE0 | ( (lba >> 24) & 0x0F));

    port_outb(0x1F2, 1);
    port_outb(0x1F3, lba & 0xFF);
    port_outb(0x1F4, (lba >> 8) & 0xFF);
    port_outb(0x1F5, (lba >> 16) & 0xFF);

    port_outb(0x1F7, 0x20);         // READ SECTOR CMD

    while( port_inb(0x1F7) & 0x80 );

    for(int i = 0; i < 256; i++)
    {
        ((uint16_t*) buffer)[i] = port_inw(0x1F0);
    }
}

void disk_write(uint32_t lba, void* buffer) {
    port_outb(0x1F6, 0xE0 | ( (lba >> 24) & 0x0F));
    
    port_outb(0x1F2, 1);
    port_outb(0x1F3, lba & 0xFF);
    port_outb(0x1F4, (lba >> 8) & 0xFF);
    port_outb(0x1F5, (lba >> 16) & 0xFF);

    port_outb(0x1F7, 0x30);         // WRITE SECTOR CMD

    while( port_inb(0x1F7) & 0x80 );
    while( !(port_inb(0x1F7) & 0x08) );
    char* b = (char*) buffer;

    for(int i = 0; i < 256; i++)
    {
        port_outw(0x1F0, ((uint16_t*) buffer)[i]);
    }

    while( port_inb(0x1F7) & 0x80 );
}

void disk_write_addr(uint32_t addr, void* buffer, size_t size) {
    int j = 0;
    char* _b = (char*) malloc( 512 );
    while( size > 0 )
    {
        uint32_t lba = (int)(addr / 512);
        uint32_t offset = addr % 512;
        port_outb(0x1F6, 0xE0 | ( (lba >> 24) & 0x0F));
    
        port_outb(0x1F2, 1);
        port_outb(0x1F3, lba & 0xFF);
        port_outb(0x1F4, (lba >> 8) & 0xFF);
        port_outb(0x1F5, (lba >> 16) & 0xFF);

        port_outb(0x1F7, 0x20);

        while( port_inb(0x1F7) & 0x80 );

        for(int i = offset; i < 512; i++)
        {
            if( size == 0)
                break;

            _b[i] = ((uint8_t*) buffer)[j];
            j++;
            size--;
            addr++;
        }
    
        port_outb(0x1F7, 0x30);         // READ SECTOR CMD
    
        while( port_inb(0x1F7) & 0x80 );

        for(int i = 0; i < 512; i++)
        {
            port_outb(0x1F0, ((uint8_t*) _b)[i]);
        }

        port_outb(0x1F7, 0xE7);
    }
    free(_b);
}