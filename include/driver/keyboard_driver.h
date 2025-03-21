#ifndef KEYBOARD_DRIVER
#define KEYBOARD_DRIVER

#include <stdint.h>

#define KEYBOARD_KEY_PRESS      1
#define KEYBOARD_KEY_RELEASE    0

typedef void (*keyboard_event_callback)(char scancode, uint8_t key, uint8_t status);

void Keyboard_Initialize();
void Keyboard_Handle();
void Keyboard_PushEvent(keyboard_event_callback callback);
void Keyboard_PopEvent(keyboard_event_callback callback);

#endif