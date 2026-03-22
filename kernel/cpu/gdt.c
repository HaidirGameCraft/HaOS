#include "cpu.h"
#include <io.h>
#include <serial.h>

gdt32_entry_t gdt32_entries[5];
gdt64_entry_t gdt64_tss;
gdt_descriptor_t gdt_desc;
task_state_segment_t tss_entry;

void gdt_init() {

    gdt_set_entry( 0, 0x00000000, 0x0000000000000000, 0x00, 0x00 );     // Null Segment
    // Kernel Code Segment
    gdt_set_entry( 1, 0xFFFFFFFF, 0x0000000000000000, GDT_ACCESS(1, 0, 1, 1, 0, 1, 0), GDT_FLAGS(1, 0, 1) );
    // Kernel Data Segment
    gdt_set_entry( 2, 0xFFFFFFFF, 0x0000000000000000, GDT_ACCESS(1, 0, 1, 0, 0, 1, 0), GDT_FLAGS(1, 1, 0) );
    // User Code Segment
    gdt_set_entry( 3, 0xFFFFFFFF, 0x0000000000000000, GDT_ACCESS(1, DPL_RING3, 1, 1, 0, 1, 0), GDT_FLAGS(1, 0, 1) );
    // User Data Segment
    gdt_set_entry( 4, 0xFFFFFFFF, 0x0000000000000000, GDT_ACCESS(1, DPL_RING3, 1, 0, 0, 1, 0), GDT_FLAGS(1, 1, 0) );
    // Task State Segment
    gdt_set_tss_entry( sizeof( task_state_segment_t ) - 1, (qword) &tss_entry, GDT_ACCESS(1, 0, 0, 1, 0, 0, 1), 0 );

    for( int i = 0; i < 5; i++ )
    {
        dword base = gdt32_entries[i].low_base | gdt32_entries[i].mid_base << 16 | (qword)(gdt32_entries[i].high_base) << 24;
        dword limit = gdt32_entries[i].low_limit | (qword)(gdt32_entries[i].high_limit) << 16;
        serial_printf("GDT[%i] = { Base: %x, Limit: %x } \n", i, base, limit );
    }
    // TSS
    {
        qword base = gdt64_tss.low_base | gdt64_tss.mid_base << 16 | (qword)(gdt64_tss.high_base) << 24 | (qword)( gdt64_tss.offset_base3 << 32 );
        dword limit = gdt64_tss.low_limit | (qword)(gdt64_tss.high_limit) << 16;
        serial_printf("GDT[6] ( TSS ) = { Base: %x, Limit: %x } \n", base, limit );
    }

    gdt_desc.base = (qword) &gdt32_entries;
    gdt_desc.limit = sizeof( gdt32_entries ) + sizeof( gdt64_tss ) - 1;
    gdt_install( (qword) &gdt_desc );
    print("Global Descriptor Table Installed!\n");

    for( int i = 0; i < sizeof( tss_entry ); i++ )
        ((byte*) &tss_entry)[i] = 0;

    extern void tss_install( task_state_segment_t* tss );
    tss_install( &tss_entry );
}

void gdt_set_entry( int index, dword limit, qword base, byte access, byte flags )
{
    gdt32_entries[index].low_limit = limit & 0xFFFF;
    gdt32_entries[index].high_limit = ( limit >> 16 ) & 0x0F;
    gdt32_entries[index].low_base = base & 0xFFFF;
    gdt32_entries[index].mid_base = ( base >> 16 ) & 0xFF;
    gdt32_entries[index].high_base = (base >> 24) & 0xFF;
    gdt32_entries[index].access = access;
    gdt32_entries[index].flags = flags & 0x0F;
}

void gdt_set_tss_entry( dword limit, qword base, byte access, byte flags ) {
    gdt64_tss.low_limit = limit & 0xFFFF;
    gdt64_tss.high_limit = ( limit >> 16 ) & 0x0F;
    gdt64_tss.low_base = base & 0xFFFF;
    gdt64_tss.mid_base = ( base >> 16 ) & 0xFF;
    gdt64_tss.high_base = (base >> 24) & 0xFF;
    gdt64_tss.offset_base3 = ( base >> 32 ) & 0xFFFFFFFF;
    gdt64_tss.access = access;
    gdt64_tss.flags = flags & 0x0F;
    gdt64_tss.reserved = 0;
}