#ifndef PORTS
#define PORTS

#include <stdint.h>

void port_outb(uint16_t port, uint8_t data);
void port_outw(uint16_t port, uint16_t data);

uint8_t port_inb(uint16_t port);
uint16_t port_inw(uint16_t port);

#endif