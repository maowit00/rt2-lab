PICO_SDK_PATH ?= $(abspath ../.pico-sdk)

default: clean build

.PHONY: default build clean flash

build:
	echo $(PICO_SDK_PATH)
	PICO_SDK_PATH=$(PICO_SDK_PATH) cmake -S . -B build
	cmake --build build

flash: build
	picotool load -f -x build/motorboard.uf2

clean:
	rm -rf build