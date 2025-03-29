#include <driver/disk_driver.h>
#include <cpu/ports.h>

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