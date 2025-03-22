#ifndef ISR_HEADER
#define ISR_HEADER

#include <stdint.h>

typedef struct {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t reserved;
    uint8_t attr;
    uint16_t isr_high;
} __attribute__((packed)) IDTEntry_t;

typedef struct {
    uint16_t limit;
    addr_t base;
} __attribute__((packed)) IDTStruct_t;

typedef struct {
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint8_t int_number;
    uint8_t error_code;
} __attribute__((packed)) register_t;

void idt_initialize();
void idt_setEntry(int index, uint16_t kernel_cs, uint8_t attribute, addr_t isr);
extern void idt_install(addr_t isr_ptr);

extern void ISR0();
extern void ISR1();
extern void ISR2();
extern void ISR3();
extern void ISR4();
extern void ISR5();
extern void ISR6();
extern void ISR7();
extern void ISR8();
extern void ISR9();
extern void ISR10();
extern void ISR11();
extern void ISR12();
extern void ISR13();
extern void ISR14();
extern void ISR15();
extern void ISR16();
extern void ISR17();
extern void ISR18();
extern void ISR19();
extern void ISR20();
extern void ISR21();
extern void ISR22();
extern void ISR23();
extern void ISR24();
extern void ISR25();
extern void ISR26();
extern void ISR27();
extern void ISR28();
extern void ISR29();
extern void ISR30();
extern void ISR31();

extern void IRQ0();
extern void IRQ1();
extern void IRQ2();
extern void IRQ3();
extern void IRQ4();
extern void IRQ5();
extern void IRQ6();
extern void IRQ7();
extern void IRQ8();
extern void IRQ9();
extern void IRQ10();
extern void IRQ11();
extern void IRQ12();
extern void IRQ13();
extern void IRQ14();
extern void IRQ15();

extern void SYSCALL();

#endif