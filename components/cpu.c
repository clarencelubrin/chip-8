#include "cpu.h"

void CPU_CYCLE() {
    /* 
        FETCH 
        - Read the instruction that RPC is currently pointing at from the memory.
        - Instruction is 2 bytes
        - Read two successibe byte from the memory and combine them into one 16-bit instruction
    */
    uint16_t INSTR = (MEMORY[RPC] << 8) | MEMORY[RPC+1];
    RPC = RPC + 2;  // inc RPC, 2
    /* 
        DECODE 
        - CHIP-8 instructions are divided by:
            > first nibble/half byte, first hexadecimal number
            > the rest of the nibble have different meanings
                X: The second nibble. Used to look up one of the 16 registers (VX) from V0 through VF.
                Y: The third nibble. Also used to look up one of the 16 registers (VY) from V0 through VF.
                N: The fourth nibble. A 4-bit number.
                NN: The second byte (third and fourth nibbles). An 8-bit immediate number.
                NNN: The second, third and fourth nibbles. A 12-bit immediate memory address.
    */
    uint8_t n1, n2, n3, n4;
    n1 = (INSTR >> 12);           
    n2 = ((INSTR >> 8) & 0x000F); 
    n3 = ((INSTR >> 4) & 0x000F); 
    n4 = (INSTR & 0x000F);        
    
    /* 
        EXECUTE 
    */
#define CHECK_NNN(a, b, c) (n2 == a && n3 == b && n4 == c)
#define CHECK_NN(b, c) (n3 == b && n4 == c)
#define NN()  (n3 << 4 | n4)
#define NNN() (n2 << 8 | n3 << 4 | n4)

    switch(n1) {
        case 0x00:
            if(CHECK_NNN(0x0, 0xE, 0x0)) {         
            /*
                00E0
                - Initialize display
            */
                INIT_DISPLAY();
            }
            else if(CHECK_NNN(0x0, 0xE, 0xE)) {     
            /*
                00EE
                - Pop the value from the stack to the PC register.
            */
                RPC = S_POP();
            }
            else if(CHECK_NNN(0x0, 0xF, 0xF)) {     
            /*
                00FF 
                - enable HIGH_RES_MODE graphics
            */
                SDL_TOGGLE_RESOLUTION(HIGH_RES_MODE);
            }
            else if(CHECK_NNN(0x0, 0xF, 0xE)) {    
            /*
                00FE 
                - disable HIGH_RES_MODE/ switch to LOW_RES_MODE
            */
                SDL_TOGGLE_RESOLUTION(LOW_RES_MODE);
            }
            else if(n2 == 0x0 && n3 == 0xC) {     
            /*
                00CN 
                - scroll-down n by 0 to 15 pixels
            */
                for(int y = SCREEN_HEIGHT - 1; y >= 0; y--) {
                    for (int x = 0; x < SCREEN_WIDTH; x++) {
                        if (y >= n4) {
                            // Pull the pixel from 'n4' rows above
                            if (RES_MODE == LOW_RES_MODE) DISPLAY_LOW_RES[y][x] = DISPLAY_LOW_RES[y - n4][x];
                            else DISPLAY_HIGH_RES[y][x] = DISPLAY_HIGH_RES[y - n4][x];
                        } else {
                            // Top rows get cleared
                            if (RES_MODE == LOW_RES_MODE) DISPLAY_LOW_RES[y][x] = false;
                            else DISPLAY_HIGH_RES[y][x] = false;
                        }
                    }
                }
                RDRAW_FLAG = true;
            }
            else if(CHECK_NNN(0x0, 0xF, 0xB)) {     
            /*
                00FB 
                - scroll-right by 4px
            */
                for (int y = 0; y < SCREEN_HEIGHT; y++) {
                    for (int x = SCREEN_WIDTH - 1; x >= 0; x--) {
                        if (x >= 4) {
                            // Pull the pixel from 4 columns to the left
                            if (RES_MODE == LOW_RES_MODE) DISPLAY_LOW_RES[y][x] = DISPLAY_LOW_RES[y][x - 4];
                            else DISPLAY_HIGH_RES[y][x] = DISPLAY_HIGH_RES[y][x - 4];
                        } else {
                            // Left-most columns get cleared
                            if (RES_MODE == LOW_RES_MODE) DISPLAY_LOW_RES[y][x] = false;
                            else DISPLAY_HIGH_RES[y][x] = false;
                        }
                    }
                }
                RDRAW_FLAG = true;
            }
            else if(CHECK_NNN(0x0, 0xF, 0xC)) {     
            /*
                00FC 
                - scroll-left by 4px
            */
                for (int y = 0; y < SCREEN_HEIGHT; y++) {
                    for (int x = 0; x < SCREEN_WIDTH; x++) {
                        if (x < SCREEN_WIDTH - 4) {
                            // Pull the pixel from 4 columns to the right
                            if (RES_MODE == LOW_RES_MODE) DISPLAY_LOW_RES[y][x] = DISPLAY_LOW_RES[y][x + 4];
                            else DISPLAY_HIGH_RES[y][x] = DISPLAY_HIGH_RES[y][x + 4];
                        } else {
                            // Right-most columns get cleared
                            if (RES_MODE == LOW_RES_MODE) DISPLAY_LOW_RES[y][x] = false;
                            else DISPLAY_HIGH_RES[y][x] = false;
                        }
                    }
                }
                RDRAW_FLAG = true;
            } else if(CHECK_NNN(0x0, 0xF, 0xD)) {
            /*
                00FD
                - Exit the Chip 8 interpreter
            */
                EXIT_FLAG = true;
            }
            break;
        /*
            0NNN
            - 12-bit address to jump to (NNN)
        */
        case 0x01:  
            RPC = NNN();
            break;
        /*
            2NNN
            - Push the current RPC to the stack
            - Set the NNN to the RPC
        */
        case 0x02:
            S_PUSH(RPC);    
            RPC = NNN();    
            break;
        /*
            3XNN
            - skip one instruction if the value in VX is equal to NN.
        */
        case 0x03:
            if(REGISTERS[n2] == NN()) RPC += 2;
            break;
        /*
            4XNN
            - skip if VX is not equal to NN.
        */
        case 0x04:
            if(REGISTERS[n2] != NN()) RPC += 2;
            break;
        /*
            5XY0
        */
        case 0x05:
            // skips if the values in VX and VY are equal
            if(REGISTERS[n2] == REGISTERS[n3]) RPC += 2;
            break;
        /*
            6XNN (set register VX)
        */
        case 0x06:
            REGISTERS[n2] = NN();
            break;
        /*
            7XNN (add value to register VX)
        */
        case 0x07:
            REGISTERS[n2] = REGISTERS[n2] + NN(); 
            break;
        case 0x08:
            if(n4 == 0x0) {          
            /*
                8XY0
            */
                REGISTERS[n2] = REGISTERS[n3];
            } else if (n4 == 0x1) {   
            /*
                8XY1
            */
                REGISTERS[n2] |= REGISTERS[n3];
            } else if (n4 == 0x2) {   
            /*
                8XY2
            */
                REGISTERS[n2] &= REGISTERS[n3];
            } else if (n4 == 0x3) {
            /*
                8XY3
            */
                REGISTERS[n2] ^= REGISTERS[n3];
            } else if (n4 == 0x4) {  
            /*
                8XY4
            */
                uint16_t sum = REGISTERS[n2] + REGISTERS[n3];
                REGISTERS[n2] = (uint8_t)(sum & 0xFF);
                REGISTERS[RFLAG] = (sum > 0xFF) ? 1 : 0;
            } else if (n4 == 0x5) {   
            /*
                8XY5
            */
                uint16_t ans = REGISTERS[n2] - REGISTERS[n3];
                uint8_t f = (REGISTERS[n2] >= REGISTERS[n3]) ? 1 : 0;
                REGISTERS[n2] = (uint8_t)(REGISTERS[n2] - REGISTERS[n3]);
                REGISTERS[RFLAG] = f;
            } else if (n4 == 0x6) {   
            /*
                8XY6
            */
                // REGISTERS[n3] = REGISTERS[n2];      // (Optional, or configurable) Set VX to the value of VY
                uint8_t f = REGISTERS[n2] & 1;          // Set VF to 1 if the bit that was shifted out was 1, or 0 if it was 0
                REGISTERS[n2] = REGISTERS[n2] >> 1; // Shift the value of VX one bit to the right (8XY6)
                REGISTERS[RFLAG] = f;
            } else if (n4 == 0x7) {   
            /*
                8XY7
            */
                uint8_t f = (REGISTERS[n3] >= REGISTERS[n2]) ? 1 : 0;
                REGISTERS[n2] = (uint8_t)(REGISTERS[n3] - REGISTERS[n2]);
                REGISTERS[RFLAG] = f;
            } else if (n4 == 0xE) {   
            /*
                8XYE
            */
                // REGISTERS[n3] = REGISTERS[n2];      // (Optional, or configurable) Set VX to the value of VY
                uint8_t f = (REGISTERS[n2] & 0x80) ? 1 : 0;   // Set VF to 1 if the bit that was shifted out was 1, or 0 if it was 0
                REGISTERS[n2] = REGISTERS[n2] << 1; // Shift the value of VX one bit to the right left (8XYE)
                REGISTERS[RFLAG] = f;
            }
            break;
        /*
            9XY0
        */
        case 0x09:
            // skips if the values in VX and VY are not equal
            if(REGISTERS[n2] != REGISTERS[n3]) {
                RPC += 2;
            } 
            break;
        /*
            ANNN (set index register I)
        */
        case 0x0A:
            RIDX = NNN();
            break;
        /*
            BXNN (jump with offset)
        */
        case 0x0B:
            RPC = NNN() + REGISTERS[n2];
            break;  
        /*
            CXNN (random)
        */
        case 0x0C:
            REGISTERS[n2] = rand() & NN();
        case 0x0D:
            if(n4 == 0x0) {
            /*
                DXY0 (sprite vx vy 0)
                - SuperChip high resolution sprites are 16x16 pixels.
                - Stored row-wise in 32 bytes (left byte, right byte).
                - Works in BOTH low-res and high-res modes!
            */
                REGISTERS[RFLAG] = 0;
                
                // Use your dynamic macros so it wraps correctly in either mode!
                uint8_t startX = REGISTERS[n2] % SCREEN_WIDTH;
                uint8_t startY = REGISTERS[n3] % SCREEN_HEIGHT;
                
                int offset = 0;
                for (uint8_t i = 0; i < 16; i++) {
                    uint8_t y = startY + i;
                    
                    if (y >= SCREEN_HEIGHT) {
                        break; // Stop drawing if it goes off the bottom of the screen
                    }
                    
                    uint8_t row_l = MEMORY[RIDX + offset];           
                    uint8_t row_r = MEMORY[RIDX + offset + 1];       
                    uint16_t row = (row_l << 8) | row_r;

                    /* iterate over the 16 bits of the row */
                    for (uint8_t bit = 0; bit < 16; bit++) {
                        uint8_t x = startX + bit;
                        
                        if (x >= SCREEN_WIDTH) {
                            break; // Stop drawing if it goes off the right edge
                        }
                        
                        uint8_t pixel = (row >> (15 - bit)) & 0x1;
                        
                        if (pixel == 1) {
                            if (RES_MODE == LOW_RES_MODE) {
                                if (DISPLAY_LOW_RES[y][x] == 1) REGISTERS[RFLAG] = 1;
                                DISPLAY_LOW_RES[y][x] ^= 1;
                            } else {
                                if (DISPLAY_HIGH_RES[y][x] == 1) REGISTERS[RFLAG] = 1;
                                DISPLAY_HIGH_RES[y][x] ^= 1;
                            }
                        }
                    }
                    offset += 2;
                }
                RDRAW_FLAG = true;
            } 
            else {
            /*
                DXYN (display/draw)
                - draws 8px or 16px (wide) x N (tall) pixel sprite at pos VX, VY
                with data starting at the address in I, I if not changed
                X,Y  = registers
                N    = sprite height, draw sprite
            */

                REGISTERS[RFLAG] = 0;
                uint8_t startX = REGISTERS[n2] % SCREEN_WIDTH;
                uint8_t startY = REGISTERS[n3] % SCREEN_HEIGHT;

                for (uint8_t i = 0; i < n4; i++) {
                    uint8_t row = MEMORY[RIDX + i];           // read from memory
                    uint8_t y   = startY + i;                 // VY + line offset
                    
                    if (y >= SCREEN_HEIGHT) break;

                    /* iterate over the 8 bits of the row */
                    for (uint8_t bit = 0; bit < 8; bit++) {
                        uint8_t x = startX + bit;
                        if (x >= SCREEN_WIDTH) break;
                        uint8_t pixel = (row >> (7 - bit)) & 0x1;
                        if (pixel == 1) {
                            if (RES_MODE == LOW_RES_MODE) {
                                if (DISPLAY_LOW_RES[y][x] == 1) REGISTERS[RFLAG] = 1;
                                DISPLAY_LOW_RES[y][x] ^= 1;
                            } else {
                                if (DISPLAY_HIGH_RES[y][x] == 1) REGISTERS[RFLAG] = 1;
                                DISPLAY_HIGH_RES[y][x] ^= 1;
                            }
                        }
                    }
                }
                RDRAW_FLAG = true;
            }
            break;
        case 0x0E:
            int c = -1;
            uint8_t key = REGISTERS[n2] & 0xF;
            if(CHECK_NN(0x9, 0xE)) {            
            /* 
                EX9E
                - Skip one instruction if the key corresponding to the value in VX is pressed 
            */
                if(KEYPAD[key] == 1) RPC += 2;
            } else if(CHECK_NN(0xA, 0x1)) {     
            /* 
                EXA1
                - Skips if the key corresponding to the value in VX is not pressed 
            */
                if(KEYPAD[key] == 0) RPC += 2;
            }
            break;
        case 0x0F:
            if(CHECK_NN(0x0, 0x7)) {           
            /* 
                FX07
                - sets VX to the current value of the delay timer 
            */
                REGISTERS[n2] = R_DELAY_TIMER;
            } else if(CHECK_NN(0x1, 0x5)) {     
            /* 
                FX15
                - sets the delay timer to the value in VX 
            */
                R_DELAY_TIMER = REGISTERS[n2];
            } else if(CHECK_NN(0x1, 0x8)) {    
            /* 
                FX18
                - sets the sound timer to the value in VX 
            */
                R_SOUND_TIMER = REGISTERS[n2];
            } else if(CHECK_NN(0x1, 0xE)) {
            /* 
                FX1E
                - sets the sound timer to the value in VX  
            */
                // REGISTERS[RFLAG] = (RIDX + REGISTERS[n2] > 0x0FFF) ? 1 : 0;  // Overflow flag (follows the Amiga interpreter)
                RIDX += REGISTERS[n2];
            } else if (CHECK_NN(0x0, 0xA)) {    
            /*
                FX0A
            */
                /* Wait for key press AND release */
                static bool key_was_pressed = false;
                static uint8_t saved_key = 0;
                
                bool any_pressed = false;
                uint8_t current_key = 0;

                /* Check if any key is currently held down */
                for (int i = 0; i < 16; i++) {
                    if (KEYPAD[i]) {
                        any_pressed = true;
                        current_key = i;
                        break;
                    }
                }

                if (!key_was_pressed) {
                /* Waiting for a finger to press a key DOWN */
                    if (any_pressed) {
                        key_was_pressed = true;
                        saved_key = current_key;
                    }
                    RPC -= 2;                    /* Repeat the PC cycle */
                } 
                else {
                /* Waiting for the finger to lift UP */
                    if (any_pressed) {
                        RPC -= 2;
                    } else {
                        /* Finger lifted. The key is released. */
                        REGISTERS[n2] = saved_key;
                        key_was_pressed = false;  
                        /* Let the program counter increment to the next instruction */
                    }
                }
            } else if (CHECK_NN(0x2, 0x9)) {        
            /*
                FX29
                - Set the register I to the font address + the first hex character in VX
            */
                RIDX = FONT_ADDRESS + (REGISTERS[n2] & 0x0F) * FONT_BYTE_PER_DIGIT;
            } else if (CHECK_NN(0x3, 0x0)) {
            /*
                FX30
                - Set i to a large hexadecimal character based on the value of vx
            */
               RIDX = BIG_FONT_ADDRESS + ((REGISTERS[n2] & 0x0F) * BIG_FONT_BYTE_PER_DIGIT);
            } else if (CHECK_NN(0x3, 0x3)) {
            /*
                FX33 (Binary-coded decimal conversion)
                - VX -> 3 decimal digits
                - Store in MEMORY[RIDX]           
            */

                MEMORY[RIDX] = REGISTERS[n2] / 100;
                MEMORY[RIDX+1] = (REGISTERS[n2] / 10) % 10;
                MEMORY[RIDX+2] = REGISTERS[n2] % 10;
            } else if (CHECK_NN(0x5, 0x5)) {
            /*
                FX55
            */
                for(int i=0; i<=n2; i++) {
                    MEMORY[RIDX+i] = REGISTERS[i];
                }
            } else if (CHECK_NN(0x6, 0x5)) {
            /*
                FX65
            */  
                for(int i=0; i<=n2; i++) {
                    REGISTERS[i] = MEMORY[RIDX+i];
                }
            } else if (CHECK_NN(0x7, 0x5)) {
            /*
                FX75 (saveflags vx) 
                - Save v0-vX to flag registers.
            */
                for(int i=0; i<=n2; i++) {
                    RPL_FLAG[i] = REGISTERS[i];
                }
            } else if (CHECK_NN(0x8, 0x5)) {
            /*
                FX85 (loadflags vx) 
                - Restore v0-vX from flag registers.
            */
                for(int i=0; i<=n2; i++) {
                    REGISTERS[i] = RPL_FLAG[i];
                }
            }
            break;
        default:
            break;
    }
}