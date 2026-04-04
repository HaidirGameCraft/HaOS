#include "cpu.h"
#include <io.h>
#include <serial.h>
#include <config.h>
#include <string.h>

byte gdt_entries[sizeof( gdt32_entry_t ) * 5 + sizeof( gdt64_entry_t ) ];
// gdt32_entry_t gdt32_entries[5];
// gdt64_entry_t gdt64_tss;
gdt_descriptor_t gdt_desc;
task_state_segment_t tss_entry;

void gdt_init() {


    // To setup, Global Descriptor Table (GDT) we need to set 6 entries, 5 in 8 bytes and 1 in 16 bytes on gdt entry
    // On First item of entries, we need to set NULL
    // Second and Third items of entries is Kernel Segment, we need to set the limit is 0xFFFFF and base 0x00000000. after that, set the access bytes and flags
    // Fourth and Fifth items of entries is User Segment, we also setup as usual like Second and Third entry but, since it user the  Descriptor Privilege Level should be on Ring 3
    // Finally, Task State Segment should use 16 bytes with specific address and size of limitation because TSS using 64 bits

    gdt32_entry_t* gdt32_e = ( gdt32_entry_t* ) gdt_entries;
    gdt64_entry_t* gdt64_e = ( gdt64_entry_t* )( (addr_t) gdt_entries + sizeof( gdt32_entry_t ) * 5 );

    // Null Segment
    memzero( &gdt32_e[0], sizeof( gdt32_entry_t ) );

    // 0x00: Kernel Code Segment
    gdt32_e[1] = (gdt32_entry_t){
        .low_limit = 0xFFFF,
        .low_base = 0x0000,
        .mid_base = 0x0000,
        .access = GDT_ACCESS(1, 0, 1, 1, 0, 1, 0 ),
        .high_limit = 0xF,
        .flags = GDT_FLAGS(1, 0, 1),
        .high_base = 0x0000
    };

    // 0x08: Kernel Data Segment
    gdt32_e[2] = (gdt32_entry_t){
        .low_limit = 0xFFFF,
        .low_base = 0x0000,
        .mid_base = 0x0000,
        .access = GDT_ACCESS(1, 0, 1, 0, 0, 1, 0 ),
        .high_limit = 0xF,
        .flags = GDT_FLAGS(1, 1, 0),
        .high_base = 0x0000
    };

    // 0x10: User Code Segment
    gdt32_e[3] = (gdt32_entry_t){
        .low_limit = 0xFFFF,
        .low_base = 0x0000,
        .mid_base = 0x0000,
        .access = GDT_ACCESS(1, DPL_RING3, 1, 1, 0, 1, 0 ),
        .high_limit = 0xF,
        .flags = GDT_FLAGS(1, 0, 1),
        .high_base = 0x0000
    };

    // 0x18: Kernel Data Segment
    gdt32_e[4] = (gdt32_entry_t){
        .low_limit = 0xFFFF,
        .low_base = 0x0000,
        .mid_base = 0x0000,
        .access = GDT_ACCESS(1, DPL_RING3, 1, 0, 0, 1, 0 ),
        .high_limit = 0xF,
        .flags = GDT_FLAGS(1, 1, 0),
        .high_base = 0x0000
    };

    // 0x20: Task State Segment
    size_t tss_limit = sizeof( task_state_segment_t ) - 1;
    gdt64_e[0] = (gdt64_entry_t){
        .low_limit = tss_limit & 0xFFFF,
        .low_base = ((qword) &tss_entry ) & 0xFFFF,
        .mid_base = ((qword) &tss_entry >> 16) & 0xFF,
        .access = GDT_ACCESS(1, 0, 0, 1, 0, 0, 1),
        .high_limit = tss_limit >> 16 & 0xFF,
        .flags = GDT_FLAGS(0, 0, 0),    // Since this is Task State Segment, set to 0
        .high_base = ((qword) &tss_entry >> 24) & 0xFF,
        .offset_base3 = ((qword) &tss_entry >> 32) & 0xFFFFFFFF,
        .reserved = 0
    };


    for( int i = 0; i < 5; i++ )
    {
        gdt32_entry_t* entry = ((gdt32_entry_t*)( (qword) gdt_entries + sizeof( gdt32_entry_t ) * i ));
        dword base = entry->low_base | entry->mid_base << 16 | (qword)(entry->high_base) << 24;
        dword limit = entry->low_limit | (qword)(entry->high_limit) << 16;
        serial_printf("GDT[%i] = { Base: %x, Limit: %x } \n", i, base, limit );
    }
    // TSS
    {
        gdt64_entry_t* entry = ((gdt64_entry_t*)((qword) gdt_entries + sizeof( gdt32_entry_t ) * 5 ) );
        qword base = entry->low_base | entry->mid_base << 16 | (qword)(entry->high_base) << 24 | (qword)( entry->offset_base3 << 32 );
        dword limit = entry->low_limit | (qword)(entry->high_limit) << 16;
        serial_printf("GDT[6] ( TSS ) = { Base: %x, Limit: %x } \n", base, limit );
    }

    // we need to set base where entries start and the size of entries
    // after that, tell cpu to use this gdt_desc
    gdt_desc.base = (qword) &gdt_entries;
    gdt_desc.limit = sizeof( gdt_entries ) - 1;
    gdt_install( (qword) &gdt_desc );
    print("Global Descriptor Table Installed!\n");

    // Setting up Task State Segment
    memzero( &tss_entry, sizeof( task_state_segment_t ) );
    tss_entry.iopb = sizeof( task_state_segment_t );
    tss_entry.rsp0 = (qword) KERNEL_STACK_BOTTOM;



    // tell cpu to use this tss_entry
    // Index of TSS in GDT
    tss_install( 0x28 );
}
