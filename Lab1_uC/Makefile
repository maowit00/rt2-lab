
default: clean build

.PHONY: default clean flash

build:
	cmake -S . -B build
	cmake --build build

flash: build
	picotool load -x build/motorboard.uf2

clean:
	rm -rf build
