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
        return -1;
    }
    if (argc < 2) {
        printf("Error: No ROM file provided.\n");
        printf("Usage: chip8_emulator.exe <path_to_rom>\n");
        return -1;
    }

    INIT_DISPLAY();
    if (INIT_AUDIO() != 0) {
        printf("Failed to initialize audio!\n");
        return -1;
    }
    LOAD_FONTS();
    LOAD_ROM(argv[1]);       // Load ROM to memory

    RPC = ROM_STARTING_LOC;  // Starting position
    bool running = true;
    
    SDL_Event event;

    uint64_t last_timer_time = SDL_GetTicks(); 

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            KEYPAD_LISTEN(&event);
        }

        for (int i = 0; i < 20; i++) {
            if(EXIT_FLAG) return 0;
            CPU_CYCLE();
        }

        /* Enforce 60fps based on ticks (VSYNC) */
        uint64_t current_time = SDL_GetTicks();
        if (current_time - last_timer_time >= 16) { // 1000ms / 60 ≈ 16.6ms
            DELAY_TIMER_CYCLE();
            SOUND_TIMER_CYCLE();
            last_timer_time = current_time;
        }

        SDL_RENDER(RDRAW_FLAG); 
        RDRAW_FLAG = false;
    }

    // Cleanup
    CLEANUP_AUDIO();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

