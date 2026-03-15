#ifndef KEYPAD_H
#define KEYPAD_H

#ifndef LIBRARY_H
    #include <stdint.h>
    #include <stdbool.h>
    #include <windows.h>
#endif

#include <SDL3/SDL.h>

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
void KEYPAD_LISTEN(SDL_Event *event);
void PRINT_KEYPAD();

int IS_ANY_PRESSED();
int MAP_SDL_KEY_TO_CHIP8(SDL_Scancode scancode);


#endif