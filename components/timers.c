#include "timers.h"

uint8_t R_DELAY_TIMER = 0;
uint8_t R_SOUND_TIMER = 0;

void DELAY_TIMER_CYCLE() {
    if(R_DELAY_TIMER == 0) {
        return;
    }
    R_DELAY_TIMER--;
}

void SOUND_TIMER_CYCLE() {
    if(R_DELAY_TIMER == 0) {
        return;
    }
    R_SOUND_TIMER--;
    // Beep sound
    // putchar('\a');
    // fflush(stdout);
    Beep(300, 300);  // Frequency: 1000 Hz, Duration: 300 ms
}