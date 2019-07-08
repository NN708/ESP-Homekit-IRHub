# ESP-Homekit-IRHub

A Homekit-enabled IR hub to control all your IR devices.

## Quick start (not really quick)

1. It is recommended to use [Ubuntu](https://ubuntu.com/), although it may work on other opreating systems.
2. Install these development tools:
```
sudo apt-get update
```
Then
```
sudo apt-get install make unrar-free autoconf automake libtool gcc g++ gperf \
    flex bison texinfo gawk ncurses-dev libexpat-dev python-dev python python-serial \
    sed git unzip bash help2man wget bzip2 libtool-bin
```
3. Clone [esp-open-sdk](https://github.com/pfalcon/esp-open-sdk/), and then compile it:
```
git clone --recursive https://github.com/pfalcon/esp-open-sdk.git
cd esp-open-sdk
make
```
4. Add generated toolchain directory (something like `/path/to/esp-open-sdk/xtensa-lx106-elf/bin`) to your PATH.
5. Download [esp-open-rtos](https://github.com/SuperHouse/esp-open-rtos/):
```
git clone --recursive https://github.com/Superhouse/esp-open-rtos.git
```
Then set environment variable SDK_PATH to the esp-open-rtos directory.
6. Clone [esp-homekit-demo](https://github.com/maximkulkin/esp-homekit-demo/) and sync all submodules:
```
git clone https://github.com/maximkulkin/esp-homekit-demo.git
cd esp-homekit-demo
git submodule update --init --recursive
```
7. Set environment variable DEMO_PATH to the esp-homekit-demo directory.
8. For NodeMCU, you need to change the flash mode:
```
export FLASH_MODE=dout
```
9. Connect your ESP8266 to your computer, then set environment variable ESPPORT pointing to your ESP8266, for example:
```
export ESPPORT=/dev/ttyUSB0
```
And make sure you can write to it by adding your user to group dialout:
```
sudo usermod -a -G dialout <your_user>
```
10. Finally, clone this repository to your computer, and change to ir_hub directory:
```
cd ESP-Homekit-IRHub/ir_hub
```
You can compile it using:
```
make
```
To flash your device, use:
```
make erase_flash
make flash
```
Then debug through serial port:
```
make monitor
```