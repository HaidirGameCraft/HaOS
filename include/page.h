#ifndef PAGING_HEADER
#define PAGING_HEADER

#include <stdint.h>

#define ATTR_PAGE_PRESENT       1 << 0
#define ATTR_PAGE_RW       1 << 1

#define PAGE_ENTRIES 1024
#define PAGE_SIZE 0x1000

typedef struct {
    uint32_t entries[PAGE_ENTRIES];
} __attribute__((packed)) page_t;

typedef struct {
    page_t entries[PAGE_ENTRIES];
} __attribute__((packed)) page_directory_t;

void Page_Initialize();
void Page_SetMapping(addr_t virtual_address, uint16_t attribute, size_t size);
void Page_ResetMapping(addr_t virtual_address, size_t size);

extern void Page_Install(uint32_t page_dir);
#endif
