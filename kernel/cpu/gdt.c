#include "cpu.h"

gdt_entry_t gdt_entries[6];
gdt_descriptor_t gdt_desc;
task_state_segment_t tss_entry;

void gdt_init() {

    gdt_set_entry( 0, 0x00000000, 0x00000000, 0x00, 0x00 );     // Null Segment
    // Kernel Code Segment
    gdt_set_entry( 1, 0xFFFFFFFF, 0x00000000, ( GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READWRITE ), ( GDT_FLAGS_GRANULARITY | GDT_FLAGS_SIZEFLAGS ) );
    // Kernel Data Segment
    gdt_set_entry( 2, 0xFFFFFFFF, 0x00000000, ( GDT_ACCESS_PRESENT | GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_READWRITE ), ( GDT_FLAGS_GRANULARITY | GDT_FLAGS_SIZEFLAGS ) );
    // User Code Segment
    gdt_set_entry( 3, 0xFFFFFFFF, 0x00000000, ( GDT_ACCESS_PRESENT | GDT_ACCESS_DPL(DPL_RING3) | GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READWRITE ), ( GDT_FLAGS_GRANULARITY | GDT_FLAGS_SIZEFLAGS ) );
    // User Data Segment
    gdt_set_entry( 4, 0xFFFFFFFF, 0x00000000, ( GDT_ACCESS_PRESENT | GDT_ACCESS_DPL(DPL_RING3) | GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_READWRITE ), ( GDT_FLAGS_GRANULARITY | GDT_FLAGS_SIZEFLAGS ) );
    // Task State Segment
    gdt_set_entry( 5, sizeof( task_state_segment_t ) - 1, (dword) &tss_entry, ( GDT_ACCESS_PRESENT | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_ACCESS), 0);

    gdt_desc.base = (dword) gdt_entries;
    gdt_desc.limit = sizeof( gdt_entry_t ) * 6 - 1;
    gdt_install( (dword) &gdt_desc );
}

void gdt_set_entry( int index, dword limit, dword base, byte access, byte flags )
{
    gdt_entries[index].low_limit = limit & 0xFFFF;
    gdt_entries[index].high_limit = ( limit >> 16 ) & 0x0F;

    gdt_entries[index].low_base = base & 0xFFFF;
    gdt_entries[index].mid_base = ( base >> 16 ) & 0xFF;
    gdt_entries[index].high_base = (base >> 24) & 0xFF;

    gdt_entries[index].access = access;
    gdt_entries[index].flags = flags & 0x0F;
}