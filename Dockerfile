FROM ubuntu:18.04
WORKDIR /repo

RUN apt-get update \
  && export DEBIAN_FRONTEND=noninteractive \
  && apt-get install -y make unrar-free autoconf automake libtool gcc g++ gperf \
    flex bison texinfo gawk ncurses-dev libexpat-dev python-dev python python-serial \
    sed git unzip bash help2man wget bzip2 libtool-bin \
  && groupadd -g 1000 docker \
  && useradd docker -u 1000 -g 1000 -s /bin/bash --no-create-home \
  && chown docker:docker /repo \
  && su docker -c "git clone --recursive https://github.com/pfalcon/esp-open-sdk.git \
    && cd /repo/esp-open-sdk \
    && make" \
  && cd /repo \
  && git clone --recursive https://github.com/Superhouse/esp-open-rtos.git \
  && git clone https://github.com/NN708/ESP-Homekit-IRHub.git \
  && cd /repo/ESP-Homekit-IRHub \
  && git submodule update --init --recursive

ENV PATH=$PATH:/repo/esp-open-sdk/xtensa-lx106-elf/bin
ENV SDK_PATH=/repo/esp-open-rtos