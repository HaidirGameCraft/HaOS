#pragma once

#include <type.h>

void ata_read_disk( dword lba, dword offset, void* buffer, dword size );