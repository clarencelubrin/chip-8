#include "keypad.h"

uint8_t KEYPAD[16] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
};

void KEYPAD_LISTEN() {
char keys[16] = {
        'X', // 0x0
        '1', // 0x1
        '2', // 0x2
        '3', // 0x3
        'Q', // 0x4
        'W', // 0x5
        'E', // 0x6
        'A', // 0x7
        'S', // 0x8
        'D', // 0x9
        'Z', // 0xA
        'C', // 0xB
        '4', // 0xC
        'R', // 0xD
        'F', // 0xE
        'V'  // 0xF
    };
    for(int i=0; i<16; i++) {
        (GetAsyncKeyState(keys[i]) & 0x8000) ?
            KEYPAD_UPDATE(i, 1) :
            KEYPAD_UPDATE(i, 0);
    }
}

void KEYPAD_UPDATE(uint8_t key, bool pressed) {
    KEYPAD[key] = pressed;
}

uint8_t IS_ANY_PRESSED() {
    for(uint8_t i=0; i<16; i++) {
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
