#include "cpu.h"

idt_entry_t idt_entries[256];
idt_desc_t idt_desc;

extern void isr_page_fault();
extern void isr_divide_zero();

void idt_init() {
    __asm__ volatile("cli");
    idt_desc.limit = sizeof( idt_entries ) - 1;
    idt_desc.base = (qword) idt_entries;

    // idt_set_entry(13, (dword) isr_page_fault,  0x08, 0x8E );
    interrupt_setEntry(0, (qword) isr_divide_zero, 0x08, 0x8E );
    interrupt_setEntry(14, (qword) isr_page_fault,  0x08, 0x8E );

    idt_install( &idt_desc );
}

void interrupt_setEntry( int index, qword offset, word segment, byte flags ) {
    idt_entries[index] = (idt_entry_t){
        .low_offset = offset & 0xFFFF,
        .code_sgement = segment,
        .reserved = 0,
        .gate_type = ( flags ) & 0b1111,
        .dpl = ( flags >> 5 ) & 0b11,
        .p = ( flags >> 7 ) & 0b1,
        .zero = 0,
        .high_offset = ( offset >> 16 ) & 0xFFFF
    };
}

void isr_handle( cpu_register_t reg ) {
    if( reg.interrupt_code == 14 )  // Page Fault
    {
        extern void page_fault_handle( cpu_register_t* reg );
        page_fault_handle( &reg );
    }
}
