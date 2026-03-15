#include "register.h"

uint8_t REGISTERS[0x10];
volatile uint8_t RPL_FLAG[8];

uint16_t RPC = 0;
uint16_t RIDX = 0;

bool RDRAW_FLAG = false;
bool EXIT_FLAG = false;