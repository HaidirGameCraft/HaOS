#include <cpu/gdt.h>
#include <cpu/tss.h>

GDTEntry_t entries[6];
GDTStruct_t gdt_desc;
TSS_t tss_desc;

void gdt_initialize() {

    gdt_setEntry(0, 0x00000000, 0x00000000, 0x00, 0x00);
    gdt_setEntry(1, 0xFFFFFFFF, 0x00000000, 0x9A, 0xC0);
    gdt_setEntry(2, 0xFFFFFFFF, 0x00000000, 0x92, 0xC0);
    gdt_setEntry(3, 0xFFFFFFFF, 0x00000000, 0xFA, 0xC0);
    gdt_setEntry(4, 0xFFFFFFFF, 0x00000000, 0xF2, 0xC0);
    gdt_setEntry(5, sizeof( TSS_t ) - 1, (uint32_t) &tss_desc, 0x89, 0x00);

    gdt_desc.limit = sizeof( entries ) - 1;
    gdt_desc.base = (addr_t) entries;

    gdt_install( (addr_t) &gdt_desc );
}

void gdt_setEntry(int index, uint32_t limit, uint32_t base, uint8_t access, uint8_t flags) {
    GDTEntry_t* entry = &entries[index];

    entry->low_limit = limit & 0xFFFF;
    entry->lflags = (( limit >> 16 ) & 0x0F);
    entry->lflags |= flags & 0xF0;

    entry->low_base = base & 0xFFFF;
    entry->mid_base = ( base >> 16 ) & 0xFF;
    entry->high_base = ( base >> 24 ) & 0xFF;

    entry->access = access;
}