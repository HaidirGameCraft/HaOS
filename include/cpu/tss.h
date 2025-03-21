#ifndef TSS_HEADER
#define TSS_HEADER

#include <stdint.h>

typedef struct {
    uint16_t link, reserved0;
    uint32_t esp0;
    uint16_t ss0, reserved1;
    uint32_t esp1;
    uint16_t ss1, reserved2;
    uint32_t esp2;
    uint16_t ss2, reserved3;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs, ldtr;
    uint32_t iopn;
    uint32_t ssp;
} __attribute__((packed)) TSS_t;


#endif