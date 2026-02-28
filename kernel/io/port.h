#pragma once

#include <type.h>

extern byte port_inb( word port );
extern word port_inw( word port );
extern dword port_ind( word port );

extern void port_outb( word port, byte data );
extern void port_outw( word port, word data );
extern void port_outd( word port, dword data );