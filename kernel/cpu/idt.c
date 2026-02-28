#include "cpu.h"
#include <serial.h>
#include <io/port.h>
#include "pic.h"

#include <time.h>
#include <driver/keyboard_driver.h>]

idt_entry_t idt_entries[256];
dword interrupt_registers[256];
idt_descriptor_t idt_desc;

extern void interrupt_x86();
void idt_init() {
    dword * interrupt_list = ( dword* ) &interrupt_x86;
    for( int i = 0; i < 48; i++ ) {
        if( i < 32 )
            serial_printf("[Interrupt Descriptor Table]: Installing ISR at %i on 0x%x\n", i, interrupt_list[i] );
        else
            serial_printf("[Interrupt Descriptor Table]: Installing IRQ at %i on 0x%x\n", i, interrupt_list[i] );
        idt_set_entry( i, interrupt_list[i], 0x08, ( IDT_FLAGS_PRESENT | IDT_FLAGS_32INTTER ) );
        interrupt_registers[i] = 0;
    }

    pic_remapped();

    idt_desc.limit = sizeof( idt_entries ) - 1;
    idt_desc.base = (dword) idt_entries;

    idt_install( (dword) &idt_desc );
    
    // Timer Programmable Interrupt for (IRQ0)
    time_init();
    keyboard_init();
    __asm__ volatile("sti");
    
}

void idt_set_entry( int index, dword offset, word code_segment, byte flags ) {
    idt_entries[index].low_offset = offset & 0xFFFF;
    idt_entries[index].high_offset = ( offset >> 16 ) & 0xFFFF;
    idt_entries[index].code_sgement = code_segment;

    idt_entries[index].reserved = 0;

    idt_entries[index].gate_type = flags & 0x0F;
    idt_entries[index].zero = 0;
    idt_entries[index].dpl = flags >> 5 & 0b11;
    idt_entries[index].p = flags >> 7 & 0b1;
}

void interrupt_register( int index, dword offset ) {
    interrupt_registers[index] = offset;
}

// ISR - Interrupt Service Request
void isr_handle( cpu_register_t creg ) {
    serial_printf("eax: 0x%x\n", creg.eax );
    serial_printf("ecx: 0x%x\n", creg.ecx );
    serial_printf("edx: 0x%x\n", creg.edx );
    serial_printf("ebx: 0x%x\n", creg.ebx );
    serial_printf("esp: 0x%x\n", creg.esp );
    serial_printf("ebp: 0x%x\n", creg.ebp );
    serial_printf("esi: 0x%x\n", creg.esi );
    serial_printf("edi: 0x%x\n", creg.edi );
    serial_printf("eflags: 0x%x\n", creg.eflags );
    serial_printf("interrupt code: 0x%x\n", creg.interrupt_code );
    serial_printf("error code: 0x%x\n", creg.error_code );
}

void irq_handle( cpu_register_t creg ) {

    // TODO
    if( creg.interrupt_code < 32 )
        return;
    pic_endofinterrupt( creg.interrupt_code - 32 );

    if( interrupt_registers[ creg.interrupt_code ] == 0 )
        return;

    void (*__handle)() = (void (*)()) interrupt_registers[ creg.interrupt_code ];
    __handle();
}