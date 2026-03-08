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
        -   VF is the flag register
 */

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdio.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <stdint.h>
#endif

#define WINDOWS     // USING WINDOWS

#ifdef WINDOWS
    #include <conio.h>
    #include <windows.h>
#endif

#include "components/cpu.h"
#include "components/display.h"
#include "components/memory.h"
#include "components/register.h"
#include "components/timers.h"

#include "assets/font.h"

unsigned __stdcall CPU_THREAD_WORKER(void* arg);
unsigned __stdcall DISPLAY_THREAD_WORKER(void* arg);
unsigned __stdcall KEYPAD_THREAD_WORKER(void* arg);

void main(int argc, char** argv) {
    SetConsoleOutputCP(437); // Sets terminal to the old DOS character set
    INIT_DISPLAY();
    LOAD_FONTS();

/* 
    ============================================================================= 
        Load ROM from the argv[1] to the MEMORY starting at ROM_STARTING_LOC
*/

    FILE *fp = fopen(argv[1], "rb");

    // Get the file length
    fseek(fp, 0, SEEK_END);
    size_t fileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    fread(&(MEMORY[ROM_STARTING_LOC]), sizeof(uint8_t), fileLen, fp);
/* 
    ============================================================================= 
*/
    int CPU_ID = 1, DISPLAY_ID = 2, KEYPAD_ID = 3;
    // uintptr_t CPU_TH = _beginthreadex(NULL, 0, CPU_THREAD_WORKER, &CPU_ID, 0, NULL);
    uintptr_t DISPLAY_TH = _beginthreadex(NULL, 0, DISPLAY_THREAD_WORKER, &DISPLAY_ID, 0, NULL);
    // uintptr_t KEYPAD_TH = _beginthreadex(NULL, 0, KEYPAD_THREAD_WORKER, &KEYPAD_ID, 0, NULL);

    // WaitForSingleObject((HANDLE)CPU_TH, INFINITE);
    WaitForSingleObject((HANDLE)DISPLAY_TH, INFINITE);
    // WaitForSingleObject((HANDLE)KEYPAD_TH, INFINITE);

    // CloseHandle((HANDLE)CPU_TH);
    CloseHandle((HANDLE)DISPLAY_TH);
    // CloseHandle((HANDLE)KEYPAD_TH);
}

unsigned __stdcall CPU_THREAD_WORKER(void *arg){
    int id = *(int*)arg;

    RPC = ROM_STARTING_LOC;  // Starting position
    while(RPC != 0xffe) {
        // CYCLE();
    }
    return 0;
}

#define INSTRUCTIONS_PER_FRAME 10

unsigned __stdcall DISPLAY_THREAD_WORKER(void *arg){
    int id = *(int*)arg;
    RPC = ROM_STARTING_LOC;  // Starting position

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    while(RPC != 0xffe) {
        KEYPAD_LISTEN();

        for (int i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
            CPU_CYCLE(); 
        }

        DELAY_TIMER_CYCLE();  // Decrement timer until 0
        SOUND_TIMER_CYCLE();  // Beep and decrement until 0

        RENDER(hConsole);

        Sleep(16); // 1000ms / 60 = 16.6ms
    }

    return 0;
}

unsigned __stdcall KEYPAD_THREAD_WORKER(void *arg){
    int id = *(int*)arg;
    int c = -1;
    while(RPC != 0xffe) {
    }
    return 0;
}
