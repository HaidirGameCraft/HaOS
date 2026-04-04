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
} __attribute__((packed)) gdt32_entry_t;

typedef struct {
    word low_limit;
    word low_base;
    byte mid_base;
    byte access;
    byte high_limit: 4;
    byte flags: 4;
    byte high_base;
    dword offset_base3;
    dword reserved;
} __attribute__((packed)) gdt64_entry_t;

typedef struct {
    word limit;
    addr_t base;
} __attribute__((packed)) gdt_descriptor_t;

void gdt_init();
extern void gdt_install( qword descriptor );
/*
    TSS = Task Stage Segment ( The information about Task )
*/
typedef struct {
    dword reserved0;
    qword rsp0;
    qword rsp1;
    qword rsp2;
    qword reserved1;
    qword ist1;
    qword ist2;
    qword ist3;
    qword ist4;
    qword ist5;
    qword ist6;
    qword ist7;
    qword reserved2;
    word reserved3;
    word iopb;
} __attribute__((packed)) task_state_segment_t;

extern void tss_install( word segment );

/*
    CPU Register List
*/
typedef struct {

    qword cr4;  // 0x00
    qword cr3;  // 0x08
    qword cr2;  // 0x10
    qword cr0;  // 0x18

    qword rax;  // 0x20
    qword rcx;  // 0x28
    qword rdx;  // 0x30
    qword rbx;  // 0x38
    qword rsp;  // 0x40
    qword rbp;  // 0x48
    qword rdi;  // 0x50
    qword rsi;  // 0x58

    qword r8;   // 0x60
    qword r9;   // 0x68
    qword r10;  // 0x70
    qword r11;  // 0x78
    qword r12;  // 0x80
    qword r13;  // 0x88
    qword r14;  // 0x90
    qword r15;  // 0x98

    qword interrupt_code;   // 0xA0
    qword error_code;   // 0xA8
    
    qword rip;          // 0xB0
    qword cs: 16;           // code segment ( 0xB8 )
    qword reserved2: 48;
    qword rflags;           // 0xC0
    qword rsp0;
    qword ss: 16;           // stack segment (0xC8)
    qword reserved3: 48;
} cpu_register_t;

/*
    LDT - Local Descriptor Table ( for Task/Thread )
*/
typedef gdt32_entry_t ldt_entry_t;

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

    // ist - Interrupt Stack Table
    // if not set, ist will ignore
    byte ist;

    /* Gate Type: The type of gate Interrupt Descriptor
     * 0b0101 - Task Gate
     * 0b0110 - 16 Bits Interrupt
     * 0b0111 - 16 Bits Trap
     * 0b1110 - 32/64 Bits Interrupt
     * 0b1111 - 32/64 Bits Trap
     */
    byte gate_type: 4;

    // always set zero
    byte zero: 1;

    /* Descriptor Privilege Level */
    byte dpl: 2;
    /* Present */
    byte p: 1;
    word mid_offset;
    dword high_offset;
    dword reserved0;
} __attribute__((packed)) idt_entry_t;

typedef gdt_descriptor_t idt_descriptor_t;

// Initialize Interrupt Descriptor Table
void idt_init();
extern void idt_install( idt_descriptor_t* descriptor );
void interrupt_setEntry( int index, qword offset, word code_segment, byte ist, byte attribute );
void interrupt_register( int index, qword offset );
