#pragma once

#include <type.h>
#define COM1_PORT   0x3F8

extern char serial_read( word port );
extern void serial_write( word port, char c );