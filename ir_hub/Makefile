PROGRAM = ir_hub
PROGRAM_SRC_DIR = . ./accessories ./utilities
PROGRAM_INC_DIR = . ./accessories ./utilities

EXTRA_COMPONENTS = \
	extras/http-parser \
	extras/pwm \
	../components/esp-wolfssl \
	../components/esp-cjson \
	../components/esp-homekit

FLASH_SIZE ?= 32

EXTRA_CFLAGS += -DHOMEKIT_SHORT_APPLE_UUIDS

include $(SDK_PATH)/common.mk

monitor:
	$(FILTEROUTPUT) --port $(ESPPORT) --baud 115200 --elf $(PROGRAM_OUT)