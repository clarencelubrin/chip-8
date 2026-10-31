#ifndef CPU_H
#define CPU_H

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdio.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <stdint.h>

    #include "register.h"
    #include "memory.h"
    #include "stack.h"
    #include "timers.h"
    #include "../assets/font.h"
    #include "display.h"
    #include "keypad.h"
#endif

/*
    CPU
    - Fetch the instruction from memory at the current PC (program counter)
    - Decode the instruction to find out what the emulator should do
    - Execute the instruction and do what it tells you
        - Timing: 1MHz to 4MHz
        - Standard speed: 700 CHIP-8 intructions per second
 */

/* Define what quirk you want by defining LEGACY, defaults to MODERN */
// #define LEGACY_Q
#ifdef LEGACY_Q
    #define VFRESET_Q
    #define MEMINC_Q
    #define SHIFT_Q
    #define JUMP_Q
    #define DISPWAIT_Q
#endif
/* Define if CLIPPING is enabled, defaults to WRAP */
#define CLIPPING_Q

#define CPU_HZ       700ULL   /* instructions per second */
#define TIMER_HZ     60ULL
#define MAX_FRAME_NS (100ULL * (uint64_t)SDL_NS_PER_SECOND / 1000ULL) /* clamp: 100 ms */

void CPU_CYCLE();

#endif