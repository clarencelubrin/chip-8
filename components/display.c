#include "display.h"


void INIT_DISPLAY() {
    for(int i=0; i<32; i++){
        for(int j=0; j<64; j++) {
            DISPLAY_MAT[i][j] = 0;
        }
    } 
}

void RENDER(HANDLE hConsole) {
    CHAR_INFO consoleBuffer[ROWS * BUFFER_WIDTH];
    COORD bufferSize = { BUFFER_WIDTH, ROWS };
    COORD charPos = { 0, 0 };
    SMALL_RECT writeRegion = { 0, 0, BUFFER_WIDTH - 1, ROWS - 1 };

    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            int leftIndex = y * BUFFER_WIDTH + (x * 2);
            int rightIndex = leftIndex + 1;

            if (DISPLAY_MAT[y][x]) {
                consoleBuffer[leftIndex].Char.AsciiChar = (char)219;
                consoleBuffer[rightIndex].Char.AsciiChar = (char)219;
                consoleBuffer[leftIndex].Attributes = FOREGROUND_GREEN;
                consoleBuffer[rightIndex].Attributes = FOREGROUND_GREEN;
            } else {
                consoleBuffer[leftIndex].Char.AsciiChar = ' ';
                consoleBuffer[rightIndex].Char.AsciiChar = ' ';
                consoleBuffer[leftIndex].Attributes = 0;
                consoleBuffer[rightIndex].Attributes = 0;
            }
        }
    }
    WriteConsoleOutput(hConsole, consoleBuffer, bufferSize, charPos, &writeRegion);
}