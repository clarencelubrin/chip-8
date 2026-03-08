#ifndef FONT_H
#define FONT_H

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdint.h>
#endif

#include "memory.h"

/*
    Font
    - Characters are 4px by 5px
    - drawn just like regular sprites
    - store in 0x050 – 0x09F
*/

extern const uint8_t fonts[80];
#define FONT_ADDRESS 0x050
#define FONT_BYTE_PER_DIGIT 5

#define LOAD_FONTS()                            \
    for(int i=0; i<80; i++) {                   \
        MEMORY[i + FONT_ADDRESS] = fonts[i];    \
    }
    
#endif