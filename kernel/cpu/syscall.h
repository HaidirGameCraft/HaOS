#pragma once

#include <type.h>
#include <fs/fat.h>

extern void read( fat_file_ptr* fp, char* buffer, size_t size );
extern fat_file_ptr* open( const char* filename );
extern void close( fat_file_ptr* fp );
