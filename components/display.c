#include "display.h"

bool DISPLAY_LOW_RES[SCREEN_HEIGHT_LOW_RES][SCREEN_WIDTH_LOW_RES];
bool DISPLAY_HIGH_RES[SCREEN_HEIGHT_HIGH_RES][SCREEN_WIDTH_HIGH_RES];

res_mode_t RES_MODE = LOW_RES_MODE;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

uint32_t pixelBuffer[128 * 64];

void INIT_DISPLAY() {
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
    if(RES_MODE == LOW_RES_MODE) {
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                pixelBuffer[(y * 64) + x] = DISPLAY_LOW_RES[y][x] ? 0xFFFFFFFF : 0x000000FF;
            }
        }
    } else {
        for (int y = 0; y < 64; y++) {
            for (int x = 0; x < 128; x++) {
                pixelBuffer[(y * 128) + x] = DISPLAY_HIGH_RES[y][x] ? 0xFFFFFFFF : 0x000000FF;
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
    
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

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