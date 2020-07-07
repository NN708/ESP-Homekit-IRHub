# ESP-Homekit-IRHub

A Homekit-enabled IR hub to control all your IR devices.

<p align="center"><img src="https://gitee.com/nn708/images/raw/master/ir_hub.jpg" width=300></p>

## Usage
### Use Docker
1. Install [Docker](https://www.docker.com) on your host machine.
2. Pull and run the Docker image:
    ```
    docker run -it nn708/esp-homekit-irhub
    ```
    > If your host machine runs Linux, you can add parameter `--device=/dev/<tty_on_your_host_machine>:/dev/ttyUSB0 -e ESPPORT=/dev/ttyUSB0` to expose your ESP8266 device to the container. Add `-e FLASH_MODE=dout` if you are using NodeMCU.
3. In the Docker container, run
    ```
    cd ESP-Homekit-IRHub/ir_hub
    make
    ```
    to compile the code. To flash your device, use:
    ```
    make erase_flash
    make flash
    ```
    Then debug through serial port:
    ```
    make monitor
    ```
    > If you are using Windows or macOS, copy the directory `/repo/ESP-Homekit-IRHub` onto your host machine and flash it outside of the container.

### Install Manually
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
    Then set environment variable ESP_OPEN_RTOS to the esp-open-rtos directory.
6. For NodeMCU, you need to change the flash mode:
    ```
    export FLASH_MODE=dout
    ```
7. Connect your ESP8266 to your computer, then set environment variable ESPPORT pointing to your ESP8266, for example:
    ```
    export ESPPORT=/dev/ttyUSB0
    ```
    And make sure you can write to it by adding your user to group dialout:
    ```
    sudo usermod -a -G dialout <your_user>
    ```
8. Clone this repository and sync all submodules:
    ```
    git clone https://github.com/NN708/ESP-Homekit-IRHub.git
    cd ESP-Homekit-IRHub
    git submodule update --init --recursive
    ```
9. Change to ir_hub directory:
    ```
    cd ir_hub
    ```
    Then you can compile it using:
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

## To-do
+ Store accessory information in flash, to make users able to add or delete accessories without editing source code.
+ Add more accessories, and let users download accessory configurations from online servers.

## License
Licensed under the [MIT](https://opensource.org/licenses/MIT) license.