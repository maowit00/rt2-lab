#!/bin/sh

cd "$(dirname "$0")"

wget https://raw.githubusercontent.com/raspberrypi/pico-setup/master/pico_setup.sh

chmod +x pico_setup.sh

./pico_setup.sh
