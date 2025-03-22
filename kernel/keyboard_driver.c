#include <cpu/cpu.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <driver/keyboard_driver.h>
#include <driver/vga_driver.h>

#define __SYSCALL__FUNCTION__
#include <syscall.h>

char keys[256];
uint8_t key_status;
char* key_char = "\0\0271234567890-=\x08\x09QWERTYUIOP[]\n\0ASDFGHJKL;'`\0\\ZXCVBNM,./\0*\0 \0";

keyboard_event_callback* key_event_callback_array;

void Keyboard_Initialize() {
    IRQ_Install(33, (addr_t) &Keyboard_Handle);

    key_event_callback_array = (keyboard_event_callback*) malloc(256 * sizeof( keyboard_event_callback ));
    memset(key_event_callback_array, 0, 256 * sizeof( keyboard_event_callback ));
}

void Keyboard_Handle() {
    uint8_t scancode = port_inb(0x60);
    uint8_t key;
    char sc;
        // Key Press
    if( scancode < 0x80 )
    {
        keys[scancode] = 1;
        key = scancode;
        sc = key_char[scancode];
    }
    else // Key Release
    {
        keys[scancode - 0x80] = 0;
        key = scancode - 0x80;
        sc = key_char[scancode - 0x80];
    }

    uint8_t key_status = scancode < 0x80 ? KEYBOARD_KEY_PRESS : KEYBOARD_KEY_RELEASE;

    for(int i = 0; i < 256; i++)
    {
        if( key_event_callback_array[i] != 0 )
        {
            key_event_callback_array[i](sc, key, key_status);
        }
    }
}

void Keyboard_PushEvent(keyboard_event_callback callback) {
    for(int i = 0; i < 256; i++)
    {
        if( key_event_callback_array[i] == 0)
        {
            key_event_callback_array[i] = callback;
            break;
        }
    }
}
void Keyboard_PopEvent(keyboard_event_callback callback) {
    for(int i = 0; i < 256; i++)
    {
        if( key_event_callback_array[i] != callback )
            continue;

        key_event_callback_array[i] = 0;
        break;
    }
}

WORD    sys_getkeypress() {
    return port_inb(0x60);
}

WORD    sys_getkeystatus() {
    return port_inb(0x64);
}