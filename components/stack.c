#include "stack.h"
#include <stdio.h>

uint16_t STACK[STACK_SIZE];
size_t TOS = 0;

bool S_PUSH(uint16_t address) {
    if (TOS >= STACK_SIZE) {
        fprintf(stderr, "Error: CHIP-8 stack overflow\n");
        return false;
    }

    STACK[TOS++] = address;
    return true;
}

bool S_POP(uint16_t *address) {
    if (TOS == 0) {
        fprintf(stderr, "Error: CHIP-8 stack underflow\n");
        return false;
    }

    *address = STACK[--TOS];
    return true;
}