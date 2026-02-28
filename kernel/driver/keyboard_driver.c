#include "keyboard_driver.h"
#include <cpu/ps2.h>
#include <cpu/cpu.h>
#include <io/port.h>
#include <serial.h>

char key_char = 0;
byte ps2_is_output_serve = 1;

const char* scancode_set_1 = "\0\n1234567890-=\x08\x09QWERTYUIOP[]\n\0ASDFGHJKL;'`\0\\ZXCVBNM,./\0*\0 \0";

void keyboard_init() {
    interrupt_register( 33, (dword) keyboard_handle );
}

void keyboard_handle() {
    // Check if the Outbuf is empty
    byte status = port_inb( PS2_STATUS );

    // if bit 0 status is set to 1, meaning that the data provide
    ps2_is_output_serve = status & PS2_STATUS_OUTBUFBIT;
}

char keyboard_getchar() {
    byte scancode = port_inb( PS2_DATA );
    ps2_is_output_serve = 0;
    return (scancode & 0x80) ? 0 : scancode_set_1[scancode & 0x7F];
}

byte keyboard_is_output_serve() {
    byte status = port_inb( PS2_STATUS );
    return status & PS2_STATUS_OUTBUFBIT;
}