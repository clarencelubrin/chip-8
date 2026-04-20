# CHIP-8 Emulator in C

This is a CHIP-8 Emulator with SuperChip support. It is backwards compatible with the original CHIP-8 instruction set. It uses SDL2 for the graphics library.

## Specifications
- Memory: 4KB RAM
- Display: 64x32 pixels monochrome
- Supports high-res mode (128x64) pixel graphics mode
- Program Counter (PC)
- 16-bit index register (IR) which is used to point at locations in memory
- Stack: 16-bit addresses, used to call subroutines/functions and return from them
- 8-bit delay timer, decremented at 60Hz until reaches to 0
- 8-bit sound timer, gives off beeping sound as long as it's not 0
- 16 8-bit general purpose variable registers numbered 0 to F hexadecimals (V0-VF)
- VF is the flag register

## Build

It is required to have CL installed in your Windows machine in order to build from source.

Build from source.
```
./build.bat
```

Run the executable.
```
cd ./build
./chip8.exe <ROM_FILE>
```

## Citations

https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#what-next
https://johnearnest.github.io/Octo/docs/SuperChip.html
https://chip8.gulrak.net/