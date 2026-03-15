#include "keypad.h"

uint8_t KEYPAD[16] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
};

void KEYPAD_LISTEN(SDL_Event *event) {
    
    /* Only care if the event is a key being pressed or released */
    if (event->type == SDL_EVENT_KEY_DOWN || event->type == SDL_EVENT_KEY_UP) {
        
        /* Determine if we should store a 1 (pressed) or 0 (released) */
        bool is_pressed = (event->type == SDL_EVENT_KEY_DOWN);
        
        /* Match the physical key scancode to the CHIP-8 hex keypad */
        switch (event->key.scancode) {
            // Row 1
            case SDL_SCANCODE_1: KEYPAD[0x1] = is_pressed; break;
            case SDL_SCANCODE_2: KEYPAD[0x2] = is_pressed; break;
            case SDL_SCANCODE_3: KEYPAD[0x3] = is_pressed; break;
            case SDL_SCANCODE_4: KEYPAD[0xC] = is_pressed; break;

            // Row 2
            case SDL_SCANCODE_Q: KEYPAD[0x4] = is_pressed; break;
            case SDL_SCANCODE_W: KEYPAD[0x5] = is_pressed; break;
            case SDL_SCANCODE_E: KEYPAD[0x6] = is_pressed; break;
            case SDL_SCANCODE_R: KEYPAD[0xD] = is_pressed; break;

            // Row 3
            case SDL_SCANCODE_A: KEYPAD[0x7] = is_pressed; break;
            case SDL_SCANCODE_S: KEYPAD[0x8] = is_pressed; break;
            case SDL_SCANCODE_D: KEYPAD[0x9] = is_pressed; break;
            case SDL_SCANCODE_F: KEYPAD[0xE] = is_pressed; break;

            // Row 4
            case SDL_SCANCODE_Z: KEYPAD[0xA] = is_pressed; break;
            case SDL_SCANCODE_X: KEYPAD[0x0] = is_pressed; break;
            case SDL_SCANCODE_C: KEYPAD[0xB] = is_pressed; break;
            case SDL_SCANCODE_V: KEYPAD[0xF] = is_pressed; break;
            
            default: break; // Ignore any other keys
        }
    }
}

int MAP_SDL_KEY_TO_CHIP8(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_X: return 0x0;
        case SDL_SCANCODE_1: return 0x1;
        case SDL_SCANCODE_2: return 0x2;
        case SDL_SCANCODE_3: return 0x3;
        case SDL_SCANCODE_Q: return 0x4;
        case SDL_SCANCODE_W: return 0x5;
        case SDL_SCANCODE_E: return 0x6;
        case SDL_SCANCODE_A: return 0x7;
        case SDL_SCANCODE_S: return 0x8;
        case SDL_SCANCODE_D: return 0x9;
        case SDL_SCANCODE_Z: return 0xA;
        case SDL_SCANCODE_C: return 0xB;
        case SDL_SCANCODE_4: return 0xC;
        case SDL_SCANCODE_R: return 0xD;
        case SDL_SCANCODE_F: return 0xE;
        case SDL_SCANCODE_V: return 0xF;
        default: return -1; // Not a CHIP-8 key
    }
}

void KEYPAD_UPDATE(uint8_t key, bool pressed) {
    KEYPAD[key] = pressed;
}

int IS_ANY_PRESSED() {
    for(int i=0; i<16; i++) {
        if(KEYPAD[i] == 1) {
            return i;
        }
    }
    return -1;
}
void PRINT_KEYPAD() {
    for(int i=0; i<16; i++) {
        printf("%d ", KEYPAD[i]);
    }
    printf("%s\n", IS_ANY_PRESSED() != -1 ? "PRESSED" : "NOT PRESSED");
}
