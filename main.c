/*
    Chip-8 Emulator
    
    @author Lubrin, Clarence, U.

    Guide by: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

    Specifications
    - Memory: 4KB RAM
    - Display: 64x32 pixels monochrome
    - Program Counter (PC)
    - 16-bit index register (i) which is used to point at locations in memory
    - Stack: 16-bit addresses, used to call subroutines/functions and return from them
    - 8-bit delay timer, decremented at 60Hz until reaches to 0
    - 8-bit sound timer, gives off beeping sound as long as it's not 0
    - 16 8-bit general purpose variable registers numbered 0 to F hexadecimals (V0-VF)
        - VF is the flag register
 */

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdio.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <stdint.h>
    #include <time.h>
    
    #include <SDL3/SDL.h>
    #include <SDL3/SDL_main.h>
#endif

#include "components/cpu.h"
#include "components/display.h"
#include "components/memory.h"
#include "components/register.h"
#include "components/timers.h"
#include "components/keypad.h"

#include "assets/font.h"

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Seed with current time

    if (SDL_INIT_GRAPHICS() != 0) {
        printf("Failed to initialize graphics!\n");
        SDL_Quit();
        return -1;
    }
    if (argc < 2) {
        printf("Error: No ROM file provided.\n");
        printf("Usage: ./chip8_emulator <path_to_rom>\n");
        return -1;
    }

    INIT_DISPLAY();
    RDRAW_FLAG = true;       // Upload the initial (blank) display to the texture
    if (INIT_AUDIO() != 0) {
        printf("Failed to initialize audio!\n");
        SDL_CLEANUP_GRAPHICS();
        SDL_Quit();
        return -1;
    }
    LOAD_FONTS();
    if (LOAD_ROM(argv[1]) != 0) {   // Load ROM to memory
        CLEANUP_AUDIO();
        SDL_CLEANUP_GRAPHICS();
        SDL_Quit();
        return -1;
    }       

    RPC = ROM_STARTING_LOC;  // Starting position
    bool running = true;
    
    SDL_Event event;

    uint64_t last_time   = SDL_GetTicksNS();
    uint64_t cpu_accum   = 0;   /* leftover nanoseconds of CPU time owed */
    uint64_t timer_accum = 0;   /* leftover nanoseconds of timer time owed */
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            KEYPAD_LISTEN(&event);
        }

        uint64_t now     = SDL_GetTicksNS();
        uint64_t elapsed = now - last_time;
        last_time = now;
        if (elapsed > MAX_FRAME_NS) elapsed = MAX_FRAME_NS; /* avoid catch-up death spiral */

        /* CPU: 700 instructions per second */
        cpu_accum += elapsed;
        uint64_t cycles = (cpu_accum * CPU_HZ) / (uint64_t)SDL_NS_PER_SECOND;
        cpu_accum -= (cycles * (uint64_t)SDL_NS_PER_SECOND) / CPU_HZ;

        for (uint64_t i = 0; i < cycles; i++) {
            if (EXIT_FLAG) { running = false; break; }
            CPU_CYCLE();
        }
        if (!running) break;

        /* Timer of 60hz, independent of render rate */
        timer_accum += elapsed;
        uint64_t ticks = (timer_accum * TIMER_HZ) / (uint64_t)SDL_NS_PER_SECOND;
        timer_accum -= (ticks * (uint64_t)SDL_NS_PER_SECOND) / TIMER_HZ;

        for (uint64_t i = 0; i < ticks; i++) {
            DELAY_TIMER_CYCLE();
            SOUND_TIMER_CYCLE();
            VBLANK_FLAG = true;
        }

        SDL_RENDER(RDRAW_FLAG);
        RDRAW_FLAG = false;
    }

    // Cleanup
    CLEANUP_AUDIO();
    SDL_CLEANUP_GRAPHICS();
    SDL_Quit();

    return 0;
}
