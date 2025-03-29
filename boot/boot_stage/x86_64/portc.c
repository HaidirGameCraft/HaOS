#include <cpu/ports.h>

void port_outb(uint16_t port, uint8_t data) {
    __asm__ volatile("out %%al, %%dx" : : "a"(data), "d"(port));
}
void port_outw(uint16_t port, uint16_t data) {
    __asm__ volatile("out %%ax, %%dx" : : "a"(data), "d"(port));
}

uint8_t port_inb(uint16_t port) {
    uint8_t r;
    __asm__ volatile("in %%dx, %%al" : "=a"(r) : "d"(port));
    return r;
}
uint16_t port_inw(uint16_t port) {
    uint16_t r;
    __asm__ volatile("in %%dx, %%ax" : "=a"(r) : "d"(port));
    return r;
}