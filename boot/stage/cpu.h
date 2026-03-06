#pragma once

#include "type.h"

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
} __attribute__((packed)) cpu_register_t;

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

typedef struct {
    word limit;
    dword base;
} __attribute__((packed)) idt_desc_t;

extern void idt_install( idt_desc_t* idt_desc );
void idt_init();
void idt_set_entry( int index, dword offset, word segment, byte attribute );