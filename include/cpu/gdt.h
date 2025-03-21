#ifndef GDT_HEADER
#define GDT_HEADER

#include <stdint.h>

typedef struct {
    uint16_t low_limit;
    uint16_t low_base;
    uint8_t mid_base;
    uint8_t access;
    uint8_t lflags;
    uint8_t high_base;
} GDTEntry_t;

typedef struct {
    uint16_t limit;
    addr_t base;
} __attribute__((packed)) GDTStruct_t;

void gdt_initialize();
void gdt_setEntry(int index, uint32_t limit, uint32_t base, uint8_t access, uint8_t flags);
extern void gdt_install(addr_t gdt_ptr);

#endif