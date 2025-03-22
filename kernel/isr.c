#include <cpu/isr.h>
#include <cpu/irq.h>
#include <driver/vga_driver.h>
#include <cpu/ports.h>
#include <stdio.h>
#include <syscall.h>

__attribute__((aligned(0x10)))
IDTEntry_t idt_entries[256];
IDTStruct_t idt_ptr;

addr_t irq_interrupt[256];

const char* MSG_INT[32] = {
    "Divide by Zero Exception",
    "Debug Exception",
    "Non-Maskable Interrupt",
    "Breakpoint Exception",
    "Overflow Exception",
    "Bound Range Exceeded",
    "Invalide Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Security Exception",
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

void remapPIC()
{
    port_outb(0x20, 0x11);
    port_outb(0xA0, 0x11);

    port_outb(0x21, 0x20);
    port_outb(0xA1, 0x28);

    port_outb(0x21, 0x04);
    port_outb(0xA1, 0x02);

    port_outb(0x21, 0x01);
    port_outb(0xA1, 0x01);
}

void idt_initialize() {
    idt_ptr.limit = sizeof( idt_entries ) - 1;
    idt_ptr.base = (addr_t) idt_entries;

    int index = 0;
    idt_setEntry(0, 0x08, 0x8E, (addr_t) ISR0);
    idt_setEntry(1, 0x08, 0x8E, (addr_t) ISR1);
    idt_setEntry(2, 0x08, 0x8E, (addr_t) ISR2);
    idt_setEntry(3, 0x08, 0x8E, (addr_t) ISR3);
    idt_setEntry(4, 0x08, 0x8E, (addr_t) ISR4);
    idt_setEntry(5, 0x08, 0x8E, (addr_t) ISR5);
    idt_setEntry(6, 0x08, 0x8E, (addr_t) ISR6);
    idt_setEntry(7, 0x08, 0x8E, (addr_t) ISR7);
    idt_setEntry(8, 0x08, 0x8E, (addr_t) ISR8);
    idt_setEntry(9, 0x08, 0x8E, (addr_t) ISR9);
    idt_setEntry(10, 0x08, 0x8E, (addr_t) ISR10);
    idt_setEntry(11, 0x08, 0x8E, (addr_t) ISR11);
    idt_setEntry(12, 0x08, 0x8E, (addr_t) ISR12);
    idt_setEntry(13, 0x08, 0x8E, (addr_t) ISR13);
    idt_setEntry(14, 0x08, 0x8E, (addr_t) ISR14);
    idt_setEntry(15, 0x08, 0x8E, (addr_t) ISR15);
    idt_setEntry(16, 0x08, 0x8E, (addr_t) ISR16);
    idt_setEntry(17, 0x08, 0x8E, (addr_t) ISR17);
    idt_setEntry(18, 0x08, 0x8E, (addr_t) ISR18);
    idt_setEntry(19, 0x08, 0x8E, (addr_t) ISR19);
    idt_setEntry(20, 0x08, 0x8E, (addr_t) ISR20);
    idt_setEntry(21, 0x08, 0x8E, (addr_t) ISR21);
    idt_setEntry(22, 0x08, 0x8E, (addr_t) ISR22);
    idt_setEntry(23, 0x08, 0x8E, (addr_t) ISR23);
    idt_setEntry(24, 0x08, 0x8E, (addr_t) ISR24);
    idt_setEntry(25, 0x08, 0x8E, (addr_t) ISR25);
    idt_setEntry(26, 0x08, 0x8E, (addr_t) ISR26);
    idt_setEntry(27, 0x08, 0x8E, (addr_t) ISR27);
    idt_setEntry(28, 0x08, 0x8E, (addr_t) ISR28);
    idt_setEntry(29, 0x08, 0x8E, (addr_t) ISR29);
    idt_setEntry(30, 0x08, 0x8E, (addr_t) ISR30);
    idt_setEntry(31, 0x08, 0x8E, (addr_t) ISR31);
    index += 32;

    remapPIC();

    idt_setEntry(index +  0, 0x08, 0x8E, (addr_t) IRQ0);
    idt_setEntry(index +  1, 0x08, 0x8E, (addr_t) IRQ1);
    idt_setEntry(index +  2, 0x08, 0x8E, (addr_t) IRQ2);
    idt_setEntry(index +  3, 0x08, 0x8E, (addr_t) IRQ3);
    idt_setEntry(index +  4, 0x08, 0x8E, (addr_t) IRQ4);
    idt_setEntry(index +  5, 0x08, 0x8E, (addr_t) IRQ5);
    idt_setEntry(index +  6, 0x08, 0x8E, (addr_t) IRQ6);
    idt_setEntry(index +  7, 0x08, 0x8E, (addr_t) IRQ7);
    idt_setEntry(index +  8, 0x08, 0x8E, (addr_t) IRQ8);
    idt_setEntry(index +  9, 0x08, 0x8E, (addr_t) IRQ9);
    idt_setEntry(index + 10, 0x08, 0x8E, (addr_t) IRQ10);
    idt_setEntry(index + 11, 0x08, 0x8E, (addr_t) IRQ11);
    idt_setEntry(index + 12, 0x08, 0x8E, (addr_t) IRQ12);
    idt_setEntry(index + 13, 0x08, 0x8E, (addr_t) IRQ13);
    idt_setEntry(index + 14, 0x08, 0x8E, (addr_t) IRQ14);
    idt_setEntry(index + 15, 0x08, 0x8E, (addr_t) IRQ15);
    
    // Syscall Entry
    idt_setEntry(128, 0x08, 0x8E, (addr_t) SYSCALL);

    idt_install((addr_t) &idt_ptr);

    __asm__ volatile("sti");
}

void idt_setEntry(int index, uint16_t kernel_cs, uint8_t attribute, addr_t isr) {
    IDTEntry_t* entry = &idt_entries[index];

    entry->isr_low = isr & 0xFFFF;
    entry->isr_high = (isr >> 16) & 0xFFFF;
    entry->reserved = 0;
    entry->kernel_cs = kernel_cs;
    entry->attr = attribute;
}

void isr_handler(register_t reg) {
    if( reg.int_number < 0x20 )
    {
        printf("Exception Message: %s\n", MSG_INT[reg.int_number]);
        printf("EAX = %x\n", reg.eax);
        printf("ECX = %x\n", reg.ecx);
        printf("EDX = %x\n", reg.edx);
        printf("EBX = %x\n", reg.ebx);
        printf("ESP = %x\n", reg.esp);
        printf("EBP = %x\n", reg.ebp);
        printf("ESI = %x\n", reg.esi);
        printf("EDI = %x\n", reg.edi);
        printf("Error Code = %x\n", reg.error_code);
        HALT();
    }
}

void irq_handler(register_t reg) {

    if( reg.int_number >= 40 ) port_outb(0xA0, 0x20);
    port_outb(0x20, 0x20);
    
    if( irq_interrupt[reg.int_number] != 0x00 )
    {
        void (*handle)() = (void (*)()) irq_interrupt[reg.int_number];
        handle();
    }
}

void IRQ_Install(int interrupt_number, addr_t function ) {
    irq_interrupt[interrupt_number] = function;
}
