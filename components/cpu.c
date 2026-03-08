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
    n1 = (INSTR >> 12);           // Get the first nibble
    n2 = ((INSTR >> 8) & 0x000F); // Get the second nibble
    n3 = ((INSTR >> 4) & 0x000F); // Get the third nibble
    n4 = (INSTR & 0x000F);        // Get the fourth nibble
    // printf("[%x] OP: %x %x%x%x", RPC, n1, n2, n3, n4);
    // NOTE: X and Y are always used to look up the values in the register
    
    /* 
        EXECUTE 
    */
#define CHECK_NNN(a, b, c) (n2 == a && n3 == b && n4 == c)
#define CHECK_NN(b, c) (n3 == b && n4 == c)
#define NN()  (n3 << 4 | n4)
#define NNN() (n2 << 8 | n3 << 4 | n4)

    switch(n1) {
        /*
            00E0, 00EE
        */
        case 0x00:
            if(CHECK_NNN(0x0, 0xE, 0x0)) {          // 00E0
                // Clear screen
                INIT_DISPLAY();
                // printf(" ; Clear screen\n");
            }
            else if(CHECK_NNN(0x0, 0xE, 0xE)) {     // 00EE
                RPC = S_POP();
                // printf(" ; return\n");
            } else {
                // printf(" ; uninplemented\n");
            }
            break;
        /*
            0NNN
        */
        case 0x01:  // 12-bit address to jump to (NNN)
            RPC = NNN();
            // printf(" ; Jump to %x\n", NNN());
            break;
        /*
            2NNN
        */
        case 0x02:
            S_PUSH(RPC);    // Push the current RPC to the stack
            RPC = NNN();    // Set the NNN to the RPC
            // printf(" ; subroutine\n");
            break;
        /*
            3XNN
        */
        case 0x03:
            // skip one instruction if the value in VX is equal to NN.
            if(REGISTERS[n2] == NN()) {
                RPC += 2;
            }
            break;
        /*
            4XNN
        */
        case 0x04:
            // skip if VX is not equal to NN.
            if(REGISTERS[n2] != NN()) {
                RPC += 2;
            }
            break;
        /*
            5XY0
        */
        case 0x05:
            // skips if the values in VX and VY are equal
            if(REGISTERS[n2] == REGISTERS[n3]) {
                RPC += 2;
            }      
            break;
        /*
            6XNN (set register VX)
        */
        case 0x06:
            REGISTERS[n2] = NN();
            // printf(" ; Set V%x to %x\n", n2, NN());
            break;
        /*
            7XNN (add value to register VX)
        */
        case 0x07:
            REGISTERS[n2] = REGISTERS[n2] + NN(); 
            // printf(" ; add V%x, %x\n", n2, NN());
            break;
        /*
            8XY0, 8XY1, 8XY2, 8XY3, 8XY4, 8XY5, 8XY6, 8XY7, 8XYE,
        */
        case 0x08:
            if(n4 == 0x0) {           // 8XY0
                REGISTERS[n2] = REGISTERS[n3];
            } else if (n4 == 0x1) {   // 8XY1
                REGISTERS[n2] |= REGISTERS[n3];
            } else if (n4 == 0x2) {   // 8XY2
                REGISTERS[n2] &= REGISTERS[n3];
            } else if (n4 == 0x3) {   // 8XY3
                REGISTERS[n2] ^= REGISTERS[n3];
            } else if (n4 == 0x4) {   // 8XY4
                uint16_t sum = REGISTERS[n2] + REGISTERS[n3];
                REGISTERS[n2] = (uint8_t)(sum & 0xFF);
                REGISTERS[RFLAG] = (sum > 0xFF) ? 1 : 0;
            } else if (n4 == 0x5) {   // 8XY5
                uint16_t ans = REGISTERS[n2] - REGISTERS[n3];
                uint8_t f = (REGISTERS[n2] >= REGISTERS[n3]) ? 1 : 0;
                REGISTERS[n2] = (uint8_t)(REGISTERS[n2] - REGISTERS[n3]);
                REGISTERS[RFLAG] = f;
            } else if (n4 == 0x6) {   // 8XY6
                REGISTERS[n2] = REGISTERS[n3];      // (Optional, or configurable) Set VX to the value of VY
                uint8_t f = REGISTERS[n2] & 1;          // Set VF to 1 if the bit that was shifted out was 1, or 0 if it was 0
                REGISTERS[n2] = REGISTERS[n2] >> 1; // Shift the value of VX one bit to the right (8XY6)
                REGISTERS[RFLAG] = f;
            } else if (n4 == 0x7) {   // 8XY7
                uint8_t f = (REGISTERS[n3] >= REGISTERS[n2]) ? 1 : 0;
                REGISTERS[n2] = (uint8_t)(REGISTERS[n3] - REGISTERS[n2]);
                REGISTERS[RFLAG] = f;
            } else if (n4 == 0xE) {   // 8XYE
                REGISTERS[n2] = REGISTERS[n3];      // (Optional, or configurable) Set VX to the value of VY
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
            // printf(" ; Set idx to %x\n", NNN());
            break;
        /*
            BXNN (jump with offset)
        */
        case 0x0B:
            RIDX = NNN() + REGISTERS[n2];
            // printf(" ; Set idx to %x + %x offset\n", NNN(), REGISTERS[n2]);
            break;  
        /*
            CXNN (random)
        */
        case 0x0C:
            srand(time(0)); // Seed with current time
            REGISTERS[n2] = rand() & NN();
            // printf(" ; random number generated: [%x] %x\n", n2, REGISTERS[n2]);
        /*
            DXYN (display/draw)
            - draws 8 (wide) x N (tall) pixel sprite at pos VX, VY
              with data starting at the address in I, I if not changed
            X,Y  = registers
            N    = sprite height, draw sprite
        */
        case 0x0D:
            // printf(" ; set screen\n");
            REGISTERS[RFLAG] = 0;
            for (uint8_t i = 0; i < n4; i++) {
                uint8_t row = MEMORY[RIDX + i];           // read from memory
                uint8_t y   = (REGISTERS[n3] + i) % 32;   // VY + line offset
                uint8_t x   =  REGISTERS[n2] % 64;        // VX

                /* iterate over the 8 bits of the row */
                for (uint8_t bit = 0; bit < 8; bit++) {
                    uint8_t pixel = (row >> (7 - bit)) & 0x1;
                    if (pixel == 1 && DISPLAY_MAT[y][(x + bit) % 64] == 1) {
                        REGISTERS[RFLAG] = 1;
                    }
                    DISPLAY_MAT[y][(x + bit) % 64] ^= pixel;   // XOR onto screen
                }
            }
            RDRAW_FLAG = 1;
            break;
        /*
            EX9E, EXA1
        */
        case 0x0E:
            int c = -1;
            if(CHECK_NN(0x9, 0xE)) {
            // EX9E
            // Skip one instruction if the key corresponding to the value in VX is pressed
                if(KEYPAD[REGISTERS[n2]] == 1) {
                    RPC += 2;
                }
                
            } else if(CHECK_NN(0xA, 0x1)) {
            // EXA1
            // Skips if the key corresponding to the value in VX is not pressed
                if(KEYPAD[REGISTERS[n2]] == 0) RPC += 2;
            }
            break;
        /*
            FX07, FX15, FX18, FX1E, FX0A, FX29, FX33, FX55, FX65
        */
        case 0x0F:
            if(CHECK_NN(0x0, 0x7)) {
                // FX07
                // sets VX to the current value of the delay timer
                REGISTERS[n2] = R_DELAY_TIMER;
            } else if(CHECK_NN(0x1, 0x5)) {
                // FX15
                // sets the delay timer to the value in VX
                R_DELAY_TIMER = REGISTERS[n2];
            } else if(CHECK_NN(0x1, 0x8)) {
                // FX18
                // sets the sound timer to the value in VX
                R_SOUND_TIMER = REGISTERS[n2];
            } else if(CHECK_NN(0x1, 0xE)) {
                // FX1E
                // The index register I will get the value in VX added to it.
                if(RIDX + REGISTERS[n2] > 0x0FFF) REGISTERS[RFLAG] = 1;  // Overflow flag (follows the Amiga interpreter)
                RIDX += REGISTERS[n2];
            } else if (CHECK_NN(0x0, 0xA)) {
                // FX0A
                // Wait for the key input
                RPC -= 2;   // Decrement PC until key is pressed
                uint8_t c = -1;
                while (1) {
                    // Check if any keys are on
                    if((c = IS_ANY_PRESSED()) != -1)
                        break;
                }
                RPC += 2;   // Return original incremented RPC
                REGISTERS[n2] = c;
            } else if (CHECK_NN(0x2, 0x9)) {
                // FX29
                // Set the register I to the font address + the first hex character in VX
                RIDX = FONT_ADDRESS + (n2 & 0x0F) * FONT_BYTE_PER_DIGIT;
            } else if (CHECK_NN(0x3, 0x3)) {
                // FX33 (Binary-coded decimal conversion)
                // VX -> 3 decimal digits
                // Store in MEMORY[RIDX]
                MEMORY[RIDX] = REGISTERS[n2] / 100;
                MEMORY[RIDX+1] = (REGISTERS[n2] / 10) % 10;
                MEMORY[RIDX+2] = REGISTERS[n2] % 10;
            } else if (CHECK_NN(0x5, 0x5)) {
                // FX55
                for(int i=0; i<=n2; i++) {
                    MEMORY[RIDX+i] = REGISTERS[i];
                }
            } else if (CHECK_NN(0x6, 0x5)) {
                // FX65
                for(int i=0; i<=n2; i++) {
                    REGISTERS[i] = MEMORY[RIDX+i];
                }
            }
            break;
        default:
            // printf("\n");
            break;
    }
}