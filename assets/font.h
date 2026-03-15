#ifndef FONT_H
#define FONT_H

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdint.h>
#endif

#include "../components/memory.h"

/*
    Font
    - Characters are 4px by 5px
    - drawn just like regular sprites
    - store in 0x050 – 0x0B3 (fonts)
    - store in 0x0B4 - 0x1FF (big fonts)
*/

extern const uint8_t fonts[80];
extern const uint8_t big_fonts[160];

#define FONT_ADDRESS 0x050
#define FONT_BYTE_PER_DIGIT 5

#define BIG_FONT_ADDRESS 0x0B4
#define BIG_FONT_BYTE_PER_DIGIT 10

void LOAD_FONTS();                                          
    
#endif