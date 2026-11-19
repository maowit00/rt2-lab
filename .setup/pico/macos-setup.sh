#!/bin/sh

set -xe

cd "$(dirname "$0")"

brew install cmake

brew install picotool

brew install --cask gcc-arm-embedded

git clone https://github.com/raspberrypi/pico-sdk.git --recurse-submodules pico-sdk

