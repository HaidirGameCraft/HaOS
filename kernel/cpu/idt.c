#include "cpu.h"
#include <serial.h>
#include <io/port.h>
#include "pic.h"
#include <page.h>
#include <io.h>

#include <time.h>
#include <driver/keyboard_driver.h>

idt_entry_t idt_entries[256] __attribute__((aligned(0x1000)));
qword interrupt_registers[256];
idt_descriptor_t idt_desc;

static const char* interrupt_descriptor_message[] = {
    "Divide With Zero Error",
    "Debug Exceptions",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bounds Check",
    "Invalid Opcode",
    "Coprocessor Not Available",
    "Double Fault",
    "Reserved",
    "Invalid Task State Segment",
    "Segment Not Present",
    "Stack Exception",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "Coprecessor Error",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

extern void interrupt_x86();
void idt_init() {
    __asm__ volatile("cli");
    qword * interrupt_list = ( qword* ) &interrupt_x86;
    for( int i = 0; i < 48; i++ ) {
        idt_set_entry(i, 0, 0, 0);
        // if( i < 32 )
        //     serial_printf("[Interrupt Descriptor Table]: Installing ISR at %i on 0x%x\n", i, interrupt_list[i] );
        // else
        //     serial_printf("[Interrupt Descriptor Table]: Installing IRQ at %i on 0x%x\n", i, interrupt_list[i] );
        idt_set_entry( i, interrupt_list[i], 0x08, 0x8E );
        interrupt_registers[i] = 0;
    }

    idt_desc.limit = sizeof( idt_entries ) - 1;
    idt_desc.base = (qword) &idt_entries;

    pic_remapped();
    print("Programmable Interrupt Controller Installed!\n");

    serial_printf("IDT Desc: Limit = %x\n", idt_desc.limit );
    serial_printf("IDT Desc: Base: %x\n", idt_desc.base );
    for( int i = 0; i < 256; i++ )
    {
        if( idt_entries[i].p == 0 )
            continue;

        qword base = ((qword)(idt_entries[i].high_offset & 0xFFFFFFFF) << 32) | ((dword)(idt_entries[i].mid_offset & 0xFFFF) << 16) | (idt_entries[i].low_offset & 0xFFFF);
        serial_printf("Interrupt[%i]: %x\n", i,  base);
    }

    

    print("Interrupt Descriptor Table Installed!\n");
    
    // Timer Programmable Interrupt for (IRQ0)
    time_init();
    print("Programmable Interrupt Time Installed!\n");
    keyboard_init();
    print("Keyboard Interrupt Installed!\n");


    idt_install( (qword) &idt_desc );
    __asm__ volatile("sti");
    // ((byte*) 0x400000)[0] = 1;
    // __asm__ volatile("hlt");
    serial_print("Interrupt [Done]\n");
    
}

void idt_set_entry( int index, qword offset, word code_segment, byte flags ) {
    idt_entries[index].low_offset = offset & 0xFFFF;
    idt_entries[index].mid_offset = ( offset >> 16 ) & 0xFFFF;
    idt_entries[index].high_offset = ( offset >> 32 ) & 0xFFFFFFFF;
    idt_entries[index].code_sgement = code_segment;

    idt_entries[index].ist = 0;

    idt_entries[index].gate_type = flags & 0x0F;
    idt_entries[index].zero = 0;
    idt_entries[index].dpl = flags >> 5 & 0b11;
    idt_entries[index].p = flags >> 7 & 0b1;

    idt_entries[index].reserved0 = 0;
}

void interrupt_register( int index, qword offset ) {
    interrupt_registers[index] = offset;
}

// ISR - Interrupt Service Request
void isr_handle( cpu_register_t creg ) {

    if( creg.interrupt_code == 14 ) // Page Fault
    {
        page_print();
    }

    serial_printf("Interrupt: %s\n", interrupt_descriptor_message[creg.interrupt_code] );
    serial_printf("rax: 0x%x\n", creg.rax );
    serial_printf("rcx: 0x%x\n", creg.rcx );
    serial_printf("rdx: 0x%x\n", creg.rdx );
    serial_printf("rbx: 0x%x\n", creg.rbx );
    serial_printf("rsp: 0x%x\n", creg.rsp );
    serial_printf("rbp: 0x%x\n", creg.rbp );
    serial_printf("rsi: 0x%x\n", creg.rsi );
    serial_printf("rdi: 0x%x\n", creg.rdi );
    serial_printf("rip: 0x%x\n", creg.rip );

    serial_printf("r8: 0x%x, r9: %x, r10: %x\n", creg.r8, creg.r9, creg.r10 );
    serial_printf("r11: 0x%x, r12: %x, r13: %x\n", creg.r11, creg.r12, creg.r13 );
    serial_printf("r14: 0x%x, r15: %x\n", creg.r14, creg.r15 );
    
    serial_printf("cr0: %x, cr2: %x, cr3: %x, cr4: %x\n", creg.cr0, creg.cr2, creg.cr3, creg.cr4 );
    serial_printf("interrupt code: 0x%x\n", creg.interrupt_code );
    serial_printf("error code: 0x%x\n", creg.error_code );

    serial_printf("rflags: 0x%x\n", creg.rflags );
    serial_printf("cs: 0x%x\n", creg.cs );

    // printf("Interrupt: %s\n", interrupt_descriptor_message[creg.interrupt_code] );
    // printf("eax: 0x%x\n", creg.eax );
    // printf("ecx: 0x%x\n", creg.ecx );
    // printf("edx: 0x%x\n", creg.edx );
    // printf("ebx: 0x%x\n", creg.ebx );
    // printf("esp: 0x%x\n", creg.esp );
    // printf("ebp: 0x%x\n", creg.ebp );
    // printf("esi: 0x%x\n", creg.esi );
    // printf("edi: 0x%x\n", creg.edi );
    // printf("eflags: 0x%x\n", creg.eflags );
    // printf("interrupt code: 0x%x\n", creg.interrupt_code );
    // printf("error code: 0x%x\n", creg.error_code );
    while( 1 )
        __asm__ volatile("hlt");

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