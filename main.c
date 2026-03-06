/*
    Chip-8 Emulator
    
    @author Lubrin, Clarence, U.

    Guide by: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

    Specifications
    - Memory: 4KB RAM
    - Display: 64x32 pixels monochrome
    - Program Counter (PC)
    - 16-bit index register (i) which is used to point at locations in memory
    - Stack: 16-bit addresses, used to call subroutines/functions and return from them
    - 8-bit delay timer, decremented at 60Hz until reaches to 0
    - 8-bit sound timer, gives off beeping sound as long as it's not 0
    - 16 8-bit general purpose variable registers numbered 0 to F hexadecimals (V0-VF)
        -   VF is the flag register
 */

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/*
    Memory
    - 0x000 - 0x1FF - Chip interpreter
    - 0x200+        - Chip-8 program loaded to here
 */
uint8_t MEMORY[0xFFF];     // 4096 bytes

uint16_t RPC = 0;            // PC can only address 12 bits
uint16_t RIDX = 0;           // Index register
/*
    Registers
    - 16 8-bit general purpose variable registers, numbered 0 to F in hex (V0-VF)
 */
uint8_t REGISTERS[0x10];
uint8_t RFLAG = 0;
/*
    Font
    - Characters are 4px by 5px
    - drawn just like regular sprites
    - store in 0x050 – 0x09F
*/

uint8_t fonts[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void LOAD_FONTS() {
    for(int i=0; i<80; i++) {
        MEMORY[i + 0x050] = fonts[i];
    }
}

/*
    Display (64px (wide) columns by 32px (tall) rows)
    - each pixel can be on and off

    DXYN opcode 
    - draw instruction of sprites to the screen. Sprites are (1-15 bytes tall)
    - 0 as transparent; all 1 bits will flip the pixels in the location of the screen
//               ROWS  COL
 */
bool DISPLAY_MAT[0x20][0x40];
void INIT_DISPLAY();
void SHOW_DISPLAY();
/*
    Stack
    - CHIP-8 uses it to call and return from subroutines and nothing ells
    - only saving addresses here
    - 16 two-bytes entries
 */
uint16_t STACK[16];

/*
    Timers
    - Delay timer
    - Sound timer - make the computer beep as long it's above 0
    - decrementedd by one 60 times per second.
    - independent of the speed of the fetch/decode/execute loop
 */
uint8_t R_DELAY_TIMER;
uint8_t R_SOUND_TIMER;

void sleep_1_60(void) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 16666667L; /* 1/60 s = 16,666,666.666... ns -> 16,666,667 ns */
    nanosleep(&ts, NULL);
}

/*
    Keypad
    - 16 keys labeled 0 to F, arranged in a 4x4 grid
    - uses keyboard scancodes
    1, 2, 3, 4
    q, w, e, r
    a, s, d, f
    z, x, c, v
 */

/*
    CPU
    - Fetch the instruction from memory at the current PC (program counter)
    - Decode the instruction to find out what the emulator should do
    - Execute the instruction and do what it tells you
    // - Timing: 1MHz to 4MHz
    // - Standard speed: 700 CHIP-8 intructions per second
 */

#define CLOCK_SPEED 700

void ITERATE() {
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
    printf("[%x] OP: %x %x%x%x", RPC, n1, n2, n3, n4);
    // NOTE: X and Y are always used to look up the values in the register
    /* 
        EXECUTE 
    */
#define CHECK(a, b, c) (n2 == a && n3 == b && n4 == c)
#define NN()  (n3 << 4 | n4)
#define NNN() (n2 << 8 | n3 << 4 | n4)

    switch(n1) {
        case 0x00:
            if(CHECK(0x0, 0xE, 0x0)) {   // 00E0
                // Clear screen
                INIT_DISPLAY();
                printf(" ; Clear screen\n");
            }
            else if(CHECK(0x0, 0xE, 0xE)) {   // 00EE
                // Return
                RFLAG = 1;
                printf(" ; Return\n");
            } else {
                printf(" ; uninplemented\n");
            }
            break;
        case 0x01:  // 12-bit address to jump to (NNN)
            RPC = NNN();
                printf(" ; Jump to %x\n", NNN());
            break;
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
            printf(" ; uninplemented\n");
            break;
        case 0x06:
            // 6XNN (set register VX)
            REGISTERS[n2] = NN();
            printf(" ; Set V%x to %x\n", n2, NN());
            break;
        case 0x07:
            // 7XNN (add value to register VX)
            REGISTERS[n2] += NN(); 
            printf(" ; add V%x, %x\n", n2, NN());

            break;
        case 0x0A:
            // ANNN (set index register I)
            RIDX = NNN();
            printf(" ; Set idx to %x\n", NNN());
            break;
        case 0x0D:
            // DXYN (display/draw)
            // - draws 8 (wide) x N (tall) pixel sprite at pos VX, VY
            //   with data starting at the address in I, I if not changed
            // X,Y  = registers
            // N    = sprite height, draw sprite
            printf(" ; set screen\n");
            
            for (uint8_t i = 0; i < n4; i++) {
                uint8_t row = MEMORY[RIDX + i];           // read from memory
                uint8_t y   = (REGISTERS[n3] + i) % 32;   // VY + line offset
                uint8_t x   =  REGISTERS[n2] % 64;        // VX

                /* iterate over the 8 bits of the row */
                for (uint8_t bit = 0; bit < 8; bit++) {
                    uint8_t pixel = (row >> (7 - bit)) & 0x1;
                    DISPLAY_MAT[y][(x + bit) % 64] ^= pixel;   // XOR onto screen
                }
            }
            break;
        default:
            printf("\n");
            break;
    }
}
void INIT_DISPLAY() {
    for(int i=0; i<32; i++){
        for(int j=0; j<64; j++) {
            DISPLAY_MAT[i][j] = 0;
        }
    } 
}
void SHOW_DISPLAY() {
    for(int i=0; i<32; i++){
        for(int j=0; j<64; j++) {
            printf("%s", DISPLAY_MAT[i][j] ? "█" : " ");
        }
        printf("\n");
    }
}
void main(int argc, char** argv) {
    INIT_DISPLAY();
    LOAD_FONTS();

#define ROM_STARTING_LOC 0x200
    // Load file (ROM) to memory to read
    FILE *fp = fopen(argv[1], "rb");

    // Get the file length
    fseek(fp, 0, SEEK_END);
    size_t fileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    fread(&(MEMORY[ROM_STARTING_LOC]), sizeof(uint8_t), fileLen, fp);

    RPC = ROM_STARTING_LOC;  // Starting position
    while(RPC != 0xffe) {
        ITERATE();
        SHOW_DISPLAY();
        // Wait for 1/60 seconds
        sleep_1_60();    
        system("clear");
    }
}