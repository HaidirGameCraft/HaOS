#ifndef IRQ_HEADER
#define IRQ_HEADER

#include <stdint.h>

void IRQ_Install(int interrupt_number, addr_t function );

#endif