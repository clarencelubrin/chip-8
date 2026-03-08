#ifndef STACK_H
#define STACK_H

#ifndef STDINT_H
#define STDINT_H
    #include <stdint.h>
#endif

/*
    Stack
    - CHIP-8 uses it to call and return from subroutines and nothing ells
    - only saving addresses here
    - 16 two-bytes entries
 */
#define STACK_SIZE 16
uint16_t STACK[STACK_SIZE];
size_t TOS = 0;

#define S_PUSH(E) if(TOS < STACK_SIZE) {STACK[TOS++] = (E);}
#define S_POP() ((TOS == 0) ? 0 : (STACK[--TOS]))

#endif