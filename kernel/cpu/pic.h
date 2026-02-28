#pragma once

#include <type.h>

/*
    PIC - Programmable Interrupt Controller
    Docs: SPC Specification - https://www.arl.wustk.edu/Publications/1995-99/arl9801.pdf
*/

#define PIC_MASTER  0x20
#define PIC_SLAVE   0xA0

#define PIC_COMMAND 0x00
#define PIC_DATA    0x01

// ICWN -> Initialisation Command Word N

// ICW1 Init - It it must (required)
#define ICW1_INIT   ( 1 << 4 )
// LTIM - Edge/Level Bank Select
#define ICW1_LTIM   ( 1 << 3 )
#define ICW1_ADI    ( 1 << 2 )
// SNGL - Single Of Cascade -> hardwired of Cascade
#define ICW1_SNGL   ( 1 << 1 )
// ICW4 - Initialisation Command Word 4 ( required )
#define ICW1_ICW4   ( 1 << 0 )

// IVBADDR - Interrupt Vector Base Address
#define ICW2_IVBADDR(x) (x & 0b11111 << 3)

// IRL - Interrupt Request Level
#define ICW2_IRL        0b000

#define ICW3_CASCADEMODE    (1 << 2)
// SIC - Slave Identification Code
#define ICW3_SIC(x)          (x)

// SFNM - Special Fully Nested Mode
#define ICW4_SFNM               ( 1 << 4 )
// BUF - Buffered Mode
#define ICW4_BUF                ( 1 << 3 )
// MSBUF - Master/Slave Buffered Mode
#define ICW4_MSBUF              ( 1 << 2 )
// AEOI - Automatically End of Interrupt
#define ICW4_AEOI               ( 1 << 1 )
// Indicate intel 8086 architecture based system
#define ICW4_MICROPROCESSOR     ( 1 << 0 )

void pic_endofinterrupt( byte irq );
void pic_remapped();