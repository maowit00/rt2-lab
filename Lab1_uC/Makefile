
default: clean build

.PHONY: default build clean flash

build:
	cmake -S . -B build
	cmake --build build

flash: build
	picotool load -f -x build/motorboard.uf2

clean:
	rm -rf build
