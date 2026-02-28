#include "pic.h"
#include <io/port.h>

void pic_endofinterrupt( byte irq ) {
    // 0x20 -> PIC End Of Interrupt
    if( irq >= 8 )  // Slave IRQ 
        port_outb( PIC_SLAVE | PIC_COMMAND, 0x20 );
    port_outb( PIC_MASTER | PIC_COMMAND, 0x20 );
}

void pic_remapped() {
    // Initialize The PIC Master and PIC Slave by providing ICW1_Init and ICW1_ICW4
    port_outb( PIC_MASTER, ICW1_INIT | ICW1_ICW4 );
    port_outb( PIC_SLAVE, ICW1_INIT | ICW1_ICW4 );

    // ICW 2
    port_outb( PIC_MASTER | PIC_DATA, 0x20 ); // Offset Vector for Master
    port_outb( PIC_SLAVE | PIC_DATA, 0x28 ); // Offset Vector for Slave

    // ICW 3
    port_outb( PIC_MASTER | PIC_DATA, ICW3_CASCADEMODE );
    port_outb( PIC_SLAVE | PIC_DATA, ICW3_SIC( 2 ) );

    // ICW 4
    // Both PIC using 8086 Mode and Not 8080 Mode
    port_outb( PIC_MASTER | PIC_DATA, ICW4_MICROPROCESSOR );
    port_outb( PIC_SLAVE | PIC_DATA, ICW4_MICROPROCESSOR );

    // Unmask both the PIC
    port_outb( PIC_MASTER | PIC_DATA, 0 );
    port_outb( PIC_SLAVE | PIC_DATA, 0 );
}