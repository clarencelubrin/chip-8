#ifndef STACK_H
#define STACK_H

#ifndef STDINT_H
#define STDINT_H
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>
#endif

/*
    Stack
    - CHIP-8 uses it to call and return from subroutines and nothing ells
    - only saving addresses here
    - 16 two-bytes entries
 */
#define STACK_SIZE 16

extern uint16_t STACK[STACK_SIZE];
extern size_t TOS;

bool S_PUSH(uint16_t address);
bool S_POP(uint16_t *address);

#endif