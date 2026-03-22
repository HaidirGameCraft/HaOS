#include <type.h>
#include <io/port.h>
#include <io.h>

#define ATA_PRIMARY_BUS     0x1F0
#define ATA_SECONDARY_BUS   0x170
#define ATAPIO_COMMAND      7
#define ATAPIO_STATUS       7
#define ATAPIO_DATA         0

extern void ata_init_pio( dword lba );
byte tmp_buffer[512];
void ata_read_disk( dword lba, dword offset, void* buffer, dword size ) {
    dword indexBuffer = 0;
    byte* __buf = (byte*) buffer;
    while( size > 0 )
    {
        ata_init_pio( lba );

        port_outb( ATA_PRIMARY_BUS | ATAPIO_COMMAND, 0x20 ); // 0x20 -> Read Sector
        while( port_inb( ATA_PRIMARY_BUS | ATAPIO_STATUS ) & 0x80 );

        
        word* __tmpbuf = (word*) &tmp_buffer;
        for( int i = 0; i < 256; i++ ) {
            __tmpbuf[0] = port_inw( ATA_PRIMARY_BUS | ATAPIO_DATA );
            __tmpbuf++; // increase tmpbuf by 2
        }
        
        for( int i = offset; i < 512; i++ ) {
            *__buf = ((byte*) tmp_buffer)[i];
            __buf++;
            size--;
            
            if( size == 0 )
                return;
        }
        offset = 0;
        lba += 1;
    }
}