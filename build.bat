@echo off
REM Build script for CHIP-8 emulator
rm -f -R build
mkdir build
cd build
cl ../*.c ../components/*.c ../assets/*.c -o chip8_emulator.exe