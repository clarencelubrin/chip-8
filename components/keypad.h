#ifndef KEYPAD_H
#define KEYPAD_H

#ifndef LIBRARY_H
    #include <stdint.h>
    #include <stdbool.h>
    #include <windows.h>
#endif
#pragma comment(lib, "user32.lib")
/*
    Keypad
    - 16 keys labeled 0 to F, arranged in a 4x4 grid
    - uses keyboard scancodes
    1, 2, 3, 4
    q, w, e, r
    a, s, d, f
    z, x, c, v
 */
extern uint8_t KEYPAD[16];

void KEYPAD_UPDATE(uint8_t key, bool pressed);
void KEYPAD_LISTEN();
void PRINT_KEYPAD();
uint8_t IS_ANY_PRESSED();



#endif