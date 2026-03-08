#ifndef TIMERS_H
#define TIMERS_H

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdio.h>
    #include <stdint.h>
    #include <windows.h>
#endif

/*
    Timers
    - Delay timer
    - Sound timer - make the computer beep as long it's above 0
    - decrementedd by one 60 times per second.
    - independent of the speed of the fetch/decode/execute loop
 */
extern uint8_t R_DELAY_TIMER;
extern uint8_t R_SOUND_TIMER;

#define sleep_ms(ms) (Sleep((DWORD)(ms)));

void DELAY_TIMER_CYCLE();
void SOUND_TIMER_CYCLE();

#endif