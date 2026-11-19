
default: flash

.PHONY: default build clean flash

build:
	cmake -S . -B build
	cmake --build build

flash: build
	cmake --build build
	picotool load -f -x build/motorboard.uf2

clean:
	rm -rf build
