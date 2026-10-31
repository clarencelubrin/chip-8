# Chip 8 makefile
SOURCES := main.c $(wildcard components/*.c) $(wildcard assets/*.c)
HEADERS := $(wildcard components/*.h) $(wildcard assets/*.h)

.PHONY: build clean setup

build: $(SOURCES) $(HEADERS)
	echo "Building Chip 8 emulator."
	mkdir -p build
	gcc $(SOURCES) -o build/chip8_emulator -I/usr/local/include -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wl,--enable-new-dtags -lSDL3 -lm

clean:
	rm -r build

setup:
	sudo apt update && sudo apt install build-essential git cmake pkg-config \
	libx11-dev libxext-dev libwayland-dev libxkbcommon-dev libgl1-mesa-dev \
	libpulse-dev libasound2-dev libpipewire-0.3-dev
	git clone --depth 1 https://github.com/libsdl-org/SDL.git -b release-3.2.x
	cmake -S SDL -B SDL/build -DCMAKE_BUILD_TYPE=Release
	cmake --build SDL/build -j$(nproc)
	sudo cmake --install SDL/build
	sudo ldconfig