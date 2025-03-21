#ifndef DISK_DRIVER
#define DISK_DRIVER

#include <stdint.h>

void disk_read(uint32_t lba, void* buffer);
void disk_write(uint32_t lba, void* buffer);
void disk_write_addr(uint32_t addr, void* buffer, size_t size);

#endif