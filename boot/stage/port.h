#pragma once

#include <type.h>

extern byte port_inb( word port );
extern void port_outb( word port, byte data );

extern word port_inw( word port );
extern void port_outw( word port, word data );