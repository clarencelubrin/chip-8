#ifndef REGISTER_H
#define REGISTER_H

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdint.h>
    #include <stdbool.h>
#endif

#include "keypad.h"

/*
    Registers
    - 16 8-bit general purpose variable registers, numbered 0 to F in hex (V0-VF)
 */
extern uint8_t REGISTERS[0x10];

#define RFLAG 0xF

extern uint16_t RPC;            // PC can only address 12 bits
extern uint16_t RIDX;           // Index register

/* RPL/Persisten FLAGS (SCHIP / HP-48) */
extern volatile uint8_t RPL_FLAG[8]; 

/* Implementation specific flag (cannot be accessed by the ROM program) */
extern bool RDRAW_FLAG;
extern bool EXIT_FLAG;
#endif