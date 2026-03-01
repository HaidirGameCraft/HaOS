#pragma once

#include <type.h>


/*
    GDT - Global Descriptor Table ( memory Segmentation for CPU ) 
*/

#define GDT_ACCESS_PRESENT      (1 << 7)
// Descriptor Privilage Level
#define GDT_ACCESS_DPL(x)       ((x) << 5)
#define GDT_ACCESS_DESCRIPTOR   (1 << 4)
#define GDT_ACCESS_EXECUTABLE   (1 << 3)

// Direction/Conforming bit
#define GDT_ACCESS_DC           (1 << 2)
#define GDT_ACCESS_READWRITE    (1 << 1)
#define GDT_ACCESS_ACCESS       (1 << 0)

#define GDT_FLAGS_GRANULARITY   (1 << 3)
// Set (1) -> 32 bits descriptor, Set (0) -> 16 bits descriptor
#define GDT_FLAGS_SIZEFLAGS     (1 << 2)
#define GDT_FLAGS_LONGMODE      (1 << 1)

/**
 * P - Present
 * DPL - Descriptor Privilage Level from Ring 0 -> Ring 3
 * D - Descriptor
 * E - Executable
 * DC - Direction / Conforming Bit
 * RW - Read/Write
 * A - Access
 */
#define GDT_ACCESS(P, DPL, D, E, DC, RW, A) ((P << 7) | ((DPL) << 5) | (D << 4) | (E << 3) | (DC << 2) | (RW << 1) | (A << 0))
/**
 * G - Granularity
 * S - Size of Descriptor ( 0 = 16 bits descriptor, 1 = 32 bits descriptor )
 * L - Long Mode ( 0 = 16/32 bits, 1 = 64 bits )
 */
#define GDT_FLAGS(G, S, L) ((G << 3) | (S << 2) | (L << 1) | 0 )

typedef struct {
    word low_limit;
    word low_base;
    byte mid_base;
    byte access;
    byte high_limit: 4;
    byte flags: 4;
    byte high_base;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    word limit;
    dword base;
} __attribute__((packed)) gdt_descriptor_t;

void gdt_init();
extern void gdt_install( dword descriptor );
void gdt_set_entry( int index, dword limit, dword base, byte access, byte flags );

/*
    TSS = Task Stage Segment ( The information about Task )
*/
typedef struct {
    dword link;     // Previous task that containing segment selector for tss
    dword esp0;     // Stack Pointer for Privilege Ring 0 ( Kernel )
    dword ss0;      // Segment Selector for Privilege Ring 0 ( Kernel )
    dword esp1;     // Stack Pointer for Privilege Ring 1/2 ( Device )
    dword ss1;      // Segment Selector for Privilege Ring 1/2 ( Device )
    dword esp2;     // Stack Pointer for Privilege Ring 3 ( User )
    dword ss2;      // Segment Selector for Privilege Ring 3 ( User )

    dword cr3;      // Page Directory Address
    dword eip;      // Program Counter
    dword eflags;   // Flags
    dword eax;
    dword ecx;
    dword edx;
    dword ebx;
    dword esp;
    dword ebp;
    dword esi;
    dword edi;

    dword es;       // Extra Segment
    dword cs;       // Code Segment
    dword ss;       // Stack Segment
    dword ds;       // Data Segment
    dword fs;       // General Purpose Segment
    dword gs;       // General Purpose Segment
    dword ldtr;
    dword iobp;     // IO Map Base Address
    dword ssp;      // Shadow Stack Pointer
} __attribute__((packed)) task_state_segment_t;

/*
    CPU Register List
*/
typedef struct {
    dword eax;
    dword ecx;
    dword edx;
    dword ebx;
    dword esp;
    dword ebp;
    dword esi;
    dword edi;

    dword eflags;
    dword interrupt_code;
    dword error_code;
} cpu_register_t;

/*
    LDT - Local Descriptor Table ( for Task/Thread )
*/
typedef gdt_entry_t ldt_entry_t;

/*
    IDT - Interrupt Descriptor Table ( for Interrupt )
*/

// Ring 0 for (Kernel)
#define DPL_RING0       0b00
// Ring 1 for (Device)
#define DPL_RING1       0b01
// Ring 2 for (Device)
#define DPL_RING2       0b10
// Ring 3 for ( Userspace )
#define DPL_RING3       0b11

#define IDT_FLAGS_PRESENT   ( 1 << 7 )
#define IDT_FLAGS_DPL(x)    ( x << 5 )
#define IDT_FLAGS_GATE(x)   ( (x) << 0 )

#define IDT_FLAGS_TASKGATE      0b0101
#define IDT_FLAGS_16BINTTER     0b0110
#define IDT_FLAGS_16TRAP        0b0111
#define IDT_FLAGS_32INTTER      0b1110
#define IDT_FLAGS_32TRAP        0b1111
typedef struct {
    word low_offset;
    word code_sgement;
    byte reserved;

    /* Gate Type: The type of gate Interrupt Descriptor*/
    byte gate_type: 4;
    byte zero: 1;

    /* Descriptor Privilege Level */
    byte dpl: 2;
    /* Present */
    byte p: 1;
    word high_offset;
} __attribute__((packed)) idt_entry_t;

typedef gdt_descriptor_t idt_descriptor_t;

void idt_init();
extern void idt_install( dword descriptor );
void idt_set_entry( int index, dword offset, word code_segment, byte attribute );
void interrupt_register( int index, dword offset );