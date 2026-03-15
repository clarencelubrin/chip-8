#ifndef DISPLAY_H
#define DISPLAY_H

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdint.h>
    #include <stdbool.h>
    #include <windows.h>
#endif

#include <SDL3/SDL.h>

#define SCREEN_WIDTH_LOW_RES 64
#define SCREEN_HEIGHT_LOW_RES 32
#define WINDOW_SCALE_LOW_RES 15 // Scale 64x32 up to 960x480

#define SCREEN_WIDTH_HIGH_RES 128
#define SCREEN_HEIGHT_HIGH_RES 64
#define WINDOW_SCALE_HIGH_RES 0 

#define SCREEN_WIDTH  ((RES_MODE == LOW_RES_MODE) ? SCREEN_WIDTH_LOW_RES : SCREEN_WIDTH_HIGH_RES)
#define SCREEN_HEIGHT ((RES_MODE == LOW_RES_MODE) ? SCREEN_HEIGHT_LOW_RES : SCREEN_HEIGHT_HIGH_RES)

/*
    Display (64px (wide) columns by 32px (tall) rows)
    - each pixel can be on and off

    DXYN opcode 
    - draw instruction of sprites to the screen. Sprites are (1-15 bytes tall)
    - 0 as transparent; all 1 bits will flip the pixels in the location of the screen
 */

extern bool DISPLAY_LOW_RES[SCREEN_HEIGHT_LOW_RES][SCREEN_WIDTH_LOW_RES];
extern bool DISPLAY_HIGH_RES[SCREEN_HEIGHT_HIGH_RES][SCREEN_WIDTH_HIGH_RES];

typedef enum {
    LOW_RES_MODE,
    HIGH_RES_MODE
} res_mode_t;

extern res_mode_t RES_MODE;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* texture;

void INIT_DISPLAY();

int SDL_INIT_GRAPHICS();
void SDL_UPDATE_PIXEL_BUFFER();
void SDL_RENDER(bool RDRAW_FLAG);
void SDL_TOGGLE_RESOLUTION(res_mode_t new_mode);

#endif