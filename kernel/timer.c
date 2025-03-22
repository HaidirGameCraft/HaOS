#include <timer.h>
#include <cpu/ports.h>
#include <cpu/isr.h>
#include <cpu/irq.h>
#include <stdio.h>

#define PIT_CHANNEL_0   0x40
#define PIT_CHANNEL_1   0x41
#define PIT_CHANNEL_2   0x42
#define PIT_CMD         0x43

#define CMD_CHANNEL_0 0 << 7 | 0 << 6
#define CMD_CHANNEL_1 0 << 7 | 1 << 6
#define CMD_CHANNEL_2 1 << 7 | 0 << 6

#define CMD_LATCH_COUNT 0 << 5 | 0 << 4
#define CMD_LOBYTE_ONLY 0 << 5 | 1 << 4
#define CMD_HIBYTE_ONLY 1 << 5 | 0 << 4
#define CMD_LHBYTE_ONLY CMD_LOBYTE_ONLY | CMD_HIBYTE_ONLY

#define FREQUENCY 1193182

void PIT_Handle()
{
    
}

// [https://wiki.osdev.org/Programmable_Interval_Timer]
void PIT_Init(uint16_t t)
{
    // Set PIT Handle
    IRQ_Install(32, (addr_t) PIT_Handle);

    // Set Channel 0, High/Low Byte and Rate Generate
    port_outb(0x43, CMD_CHANNEL_0 | CMD_LHBYTE_ONLY | (1 << 2) );

    port_outb(PIT_CHANNEL_0, t & 0xFF);
    port_outb(PIT_CHANNEL_0, (t >> 8) & 0xFF);
}