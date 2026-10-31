#include "display.h"

bool DISPLAY_LOW_RES[SCREEN_HEIGHT_LOW_RES][SCREEN_WIDTH_LOW_RES];      // Display buffer for low resolution mode (64x32)
bool DISPLAY_HIGH_RES[SCREEN_HEIGHT_HIGH_RES][SCREEN_WIDTH_HIGH_RES];   // Display buffer for high resolution mode (128x64)

res_mode_t RES_MODE = LOW_RES_MODE;                                     // What mode the display is currently in (low or high resolution)

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

uint32_t pixelBuffer[128 * 64];

/*
    Colors are packed according to SDL_PIXELFORMAT_RGBA32, which is
    endianness-dependent (ABGR8888 on little-endian, RGBA8888 on big-endian).
    Let SDL build the correct 32-bit values instead of hardcoding them.
*/
static uint32_t PIXEL_ON  = 0xFFFFFFFF;
static uint32_t PIXEL_OFF = 0xFF000000;
static bool PIXEL_COLORS_READY = false;

static void INIT_PIXEL_COLORS() {
    const SDL_PixelFormatDetails *format = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA32);
    if (format) {
        PIXEL_ON  = SDL_MapRGBA(format, NULL, 255, 255, 255, 255); // opaque white
        PIXEL_OFF = SDL_MapRGBA(format, NULL, 0, 0, 0, 255);       // opaque black
    }
    PIXEL_COLORS_READY = true;
}

void INIT_DISPLAY() {
    if (!PIXEL_COLORS_READY) INIT_PIXEL_COLORS();
    for (int y = 0; y < SCREEN_HEIGHT_LOW_RES; y++) {
        for (int x = 0; x < SCREEN_WIDTH_LOW_RES; x++) {
            DISPLAY_LOW_RES[y][x] = false; 
        }
    } 
    for (int y = 0; y < SCREEN_HEIGHT_HIGH_RES; y++) {
        for (int x = 0; x < SCREEN_WIDTH_HIGH_RES; x++) {
            DISPLAY_HIGH_RES[y][x] = false; 
        }
    } 
}

void SDL_UPDATE_PIXEL_BUFFER() {
    if (!PIXEL_COLORS_READY) INIT_PIXEL_COLORS();
    if(RES_MODE == LOW_RES_MODE) {
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                pixelBuffer[(y * 64) + x] = DISPLAY_LOW_RES[y][x] ? PIXEL_ON : PIXEL_OFF;
            }
        }
    } else {
        for (int y = 0; y < 64; y++) {
            for (int x = 0; x < 128; x++) {
                pixelBuffer[(y * 128) + x] = DISPLAY_HIGH_RES[y][x] ? PIXEL_ON : PIXEL_OFF;
            }
        }
    }
}
int SDL_INIT_GRAPHICS() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return -1;
    }

    RES_MODE = LOW_RES_MODE;
    
    window = SDL_CreateWindow("CHIP-8 Emulator", 
                               SCREEN_WIDTH_LOW_RES * WINDOW_SCALE_LOW_RES, 
                               SCREEN_HEIGHT_LOW_RES * WINDOW_SCALE_LOW_RES, 
                               SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        return 1;
    }
    
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        return 1;
    }

    SDL_SetRenderVSync(renderer, 1);

    SDL_SetRenderLogicalPresentation(renderer, 64, 32, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, 
                                SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!texture) {
        SDL_Log("Texture creation failed: %s", SDL_GetError());
        return 1;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    return 0;
}

void SDL_TOGGLE_RESOLUTION(res_mode_t new_mode) {
    RES_MODE = new_mode;

    INIT_DISPLAY();

    if (texture != NULL) {
        SDL_DestroyTexture(texture);
    }

    int new_width = (RES_MODE == LOW_RES_MODE) ? SCREEN_WIDTH_LOW_RES : SCREEN_WIDTH_HIGH_RES;
    int new_height = (RES_MODE == LOW_RES_MODE) ? SCREEN_HEIGHT_LOW_RES : SCREEN_HEIGHT_HIGH_RES;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, 
                                SDL_TEXTUREACCESS_STREAMING, new_width, new_height);
    if (!texture) {
        SDL_Log("Texture recreation failed: %s", SDL_GetError());
        return;
    }
    
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);

    SDL_SetRenderLogicalPresentation(renderer, new_width, new_height, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    
}

void SDL_RENDER(bool RDRAW_FLAG) {
    if (RDRAW_FLAG) {
        SDL_UPDATE_PIXEL_BUFFER();
        int pitch = (RES_MODE == LOW_RES_MODE) ? (64 * 4) : (128 * 4);
        SDL_UpdateTexture(texture, NULL, pixelBuffer, pitch);
    }

    // SDL3 Change: SDL_RenderClear only takes the renderer.
    SDL_RenderClear(renderer);

    // SDL3 Change: SDL_RenderCopy is replaced by SDL_RenderTexture.
    SDL_RenderTexture(renderer, texture, NULL, NULL);

    // Present to the user
    SDL_RenderPresent(renderer);
}