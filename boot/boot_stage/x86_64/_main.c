#include <stdint.h>

extern void Check_CPUID();
extern void Check_LongMode();
extern void Enable_Paging(uint64_t pml4t);
extern void Halt();
void main() {
    Check_CPUID();
    Check_LongMode();

    uint64_t* pml4t = (uint64_t*) 0x00400000;
    uint64_t* pdpt = (uint64_t*) 0x00401000;
    uint64_t* pdt = (uint64_t*) 0x00402000;
    uint64_t* pd = (uint64_t*) 0x00403000;

    
    for(int i = 0; i < 512; i++)
    {
        pd[i] = (i * 0x200000) | (1 << 7) | (1 << 1) | (1 << 0);
    }
    
    pml4t[0] = ((uint64_t) pdpt) | (1 << 1) | (1 << 0);
    pdpt[0] = ((uint64_t) pdt) | (1 << 1) | (1 << 0);
    pdt[0] = ((uint64_t) pd) | (1 << 1) | (1 << 0);

    Enable_Paging((uint64_t) pml4t);
    Halt();
}