#ifndef REGISTER_H
#define REGISTER_H

#ifndef LIBRARY_H
#define LIBRARY_H
    #include <stdint.h>
#endif
/*
    Registers
    - 16 8-bit general purpose variable registers, numbered 0 to F in hex (V0-VF)
 */
extern uint8_t REGISTERS[0x10];

// #define RFLAG REGISTERS[0xF]   // VF is the flag register
#define RFLAG 0xF

extern uint16_t RPC;            // PC can only address 12 bits
extern uint16_t RIDX;           // Index register
extern uint8_t RDRAW_FLAG;
#endif