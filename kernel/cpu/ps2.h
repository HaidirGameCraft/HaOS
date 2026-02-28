#pragma once

#include <type.h>

/*
    PS2 Controller
    Referrence: I8024 PS/2 Controller - https://wiki.osdev.org/I8042_PS/2_Controller
*/

#define PS2_DATA        0x60
#define PS2_STATUS      0x64
#define PS2_COMMAND     0x64

// OutBuf - Output Buffer: set (0) -> empty, set (1) -> full
#define PS2_STATUS_OUTBUFBIT   0x01
// InBuf - Input Buffer: set (0) _. empty, set (1) -> full
#define PS2_STATUS_INBUFBIT    0x02
#define PS2_STATUS_SYSFLAGBIT  0x04
#define PS2_STATUS_CMDDATABIT  0x08
#define PS2_STATUS_TIMEOUTBIT  0x40
#define PS2_STATUS_PARITYBIT   0x80

#define PS2_COMMAND_READBYTE    0x20
#define PS2_COMMAND_READN(x)    (PS2_COMMAND + x )