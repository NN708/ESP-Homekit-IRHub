FROM beetix/esp-open-rtos:latest

WORKDIR /
RUN git clone https://github.com/NN708/ESP-Homekit-IRHub.git

WORKDIR /ESP-Homekit-IRHub
RUN git submodule update --init --recursive