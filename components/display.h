#ifndef DISPLAY_H
#define DISPLAY_H

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdbool.h>
    #include <windows.h>
#endif

/*
    Display (64px (wide) columns by 32px (tall) rows)
    - each pixel can be on and off

    DXYN opcode 
    - draw instruction of sprites to the screen. Sprites are (1-15 bytes tall)
    - 0 as transparent; all 1 bits will flip the pixels in the location of the screen
 */

#define ROWS 0x20 // 32
#define COLS 0x40 // 64
#define BUFFER_WIDTH (COLS * 2)

bool DISPLAY_MAT[ROWS][COLS];

#define HIDE_CURSOR() printf("\e[?25l");
#define SHOW_CURSOR() printf("\e[?25h");
void INIT_DISPLAY();
void RENDER();

#endif