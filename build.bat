@echo off
REM Build script for CHIP-8 emulator
del build
mkdir build
cd build
copy ..\lib\x64\SDL3.dll .
cl ../*.c ../components/*.c ../assets/*.c /Fe:chip8.exe /I"../include" /link ../lib/x64/SDL3.lib /subsystem:windows
cd ..