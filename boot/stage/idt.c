#include "cpu.h"

idt_entry_t idt_entries[256];
idt_desc_t idt_desc;

extern void isr_page_fault();

void idt_init() {
    __asm__ volatile("cli");
    idt_desc.limit = sizeof( idt_entries ) - 1;
    idt_desc.base = (dword) idt_entries;

    idt_set_entry(13, (dword) isr_page_fault,  0x08, 0x8E );
    idt_set_entry(14, (dword) isr_page_fault,  0x08, 0x8E );

    idt_install( &idt_desc );
}

void idt_set_entry( int index, dword offset, word segment, byte flags ) {
    idt_entry_t* entry = &idt_entries[index];

    entry->low_offset = offset & 0xFFFF;
    entry->high_offset = ( offset >> 16) & 0xFFFF;

    entry->code_sgement = segment;
    entry->reserved = 0;
    entry->gate_type = flags & 0x0F;
    entry->zero = 0;
    entry->dpl = flags >> 5 & 0b11;
    entry->p = flags >> 7 & 0b1;
}

void isr_handle( cpu_register_t reg ) {
    if( reg.interrupt_code == 14 )  // Page Fault
    {
        extern void page_fault_handle( cpu_register_t* reg );
        page_fault_handle( &reg );
    }
}