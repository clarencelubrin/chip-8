#include "timers.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t R_DELAY_TIMER = 0;
uint8_t R_SOUND_TIMER = 0;

SDL_AudioStream* audio_stream = NULL;
SDL_AudioSpec spec;

/* Keep track of both phase and a smooth volume state */ 
float current_phase = 0.0f; 
float current_volume = 0.0f; 

int INIT_AUDIO() {
    if (!SDL_Init(SDL_INIT_AUDIO)) {
        return 1;
    }
    
    /* Audio spec settings */
    spec.freq = 44100;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 1;
    
    /* Initialize open audio device stream */
    audio_stream = SDL_OpenAudioDeviceStream(
        SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, 
        &spec, 
        NULL,  
        NULL   
    );
    
    if (!audio_stream) {
        SDL_Quit();
        return 1;
    }
    
    return 0; 
}

void SOUND_TIMER_CYCLE() {
    /*
        Maintain a small continuous buffer (~100ms)
        This keeps latency low and prevents audio crackling if the loop stutters.
    */
    int target_queued_bytes = (spec.freq / 10) * sizeof(float);
    int current_queued_bytes = SDL_GetAudioStreamAvailable(audio_stream);
    
    if (current_queued_bytes < target_queued_bytes) {
        int bytes_to_add = target_queued_bytes - current_queued_bytes;
        int samples_to_add = bytes_to_add / sizeof(float);
        
        float* buffer = (float*)malloc(samples_to_add * sizeof(float));
        float frequency = 440.0f;
        
        for (int i = 0; i < samples_to_add; i++) {
            /* Determine our target volume based on whether the timer is active */
            float target_volume = (R_SOUND_TIMER > 0) ? 0.5f : 0.0f;
            /*
                Smoothly ramp the volume (Attack/Release Envelope)
                This lerp prevents the sharp discontinuity that causes the click.
                Adjust the 0.01f multiplier to make the fade faster or slower.
            */
            current_volume += (target_volume - current_volume) * 0.01f;
            
            /*  Snap to zero if it's very quiet to avoid floating point micro-oscillations */
            if (current_volume < 0.0001f && target_volume == 0.0f) {
                current_volume = 0.0f;
            }

            /* Apply our smoothed volume to the sine wave */
            buffer[i] = current_volume * sinf(current_phase);
            
            /* Advance phase */ 
            current_phase += 2.0f * (float)M_PI * frequency / spec.freq;
            if (current_phase > 2.0f * (float)M_PI) {
                current_phase -= 2.0f * (float)M_PI;
            }
        }
        
        SDL_PutAudioStreamData(audio_stream, buffer, samples_to_add * sizeof(float));
        free(buffer);
    }

    SDL_ResumeAudioStreamDevice(audio_stream);

    /* Handle CHIP-8 timer decrement */
    if (R_SOUND_TIMER > 0) {
        R_SOUND_TIMER--;
    }
}

void DELAY_TIMER_CYCLE() {
    if (R_DELAY_TIMER > 0) {
        R_DELAY_TIMER--;
    }
}

void CLEANUP_AUDIO() {
    if (audio_stream) {
        SDL_DestroyAudioStream(audio_stream);
    }
}