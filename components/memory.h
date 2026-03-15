#ifndef MEMORY_H
#define MEMORY_H

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdint.h>
    #include <stdio.h>
#endif
/*
    Memory
    - 0x000 - 0x1FF - Chip interpreter
    - 0x200+        - Chip-8 program loaded to here
 */
extern uint8_t MEMORY[0x1000];     // 4096 bytes

int LOAD_ROM(char* location);

#define ROM_STARTING_LOC 0x200
#endif