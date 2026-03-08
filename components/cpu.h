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
    // - Timing: 1MHz to 4MHz
    // - Standard speed: 700 CHIP-8 intructions per second
 */

#define CLOCK_SPEED 700

void CPU_CYCLE();

#endif