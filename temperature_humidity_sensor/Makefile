PROGRAM = temperature_humidity_sensor

EXTRA_COMPONENTS = \
	extras/http-parser \
	extras/dhcpserver \
	extras/dht \
	../components/esp-wolfssl \
	../components/esp-cjson \
	../components/esp-homekit \
	../components/esp-wifi-config

FLASH_SIZE ?= 32

EXTRA_CFLAGS += -DHOMEKIT_SHORT_APPLE_UUIDS

include $(SDK_PATH)/common.mk

monitor:
	$(FILTEROUTPUT) --port $(ESPPORT) --baud 115200 --elf $(PROGRAM_OUT)