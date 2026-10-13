#include "register.h"

uint8_t REGISTERS[0x10];
volatile uint8_t RPL_FLAG[8];

uint16_t RPC = 0;           // Program counter register
uint16_t RIDX = 0;          // Index register

bool RDRAW_FLAG = false;    // Draw flag register
bool EXIT_FLAG = false;     // Exit flag register