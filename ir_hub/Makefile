PROGRAM = ir_hub
PROGRAM_SRC_DIR = . ./accessories ./utilities
PROGRAM_INC_DIR = . ./accessories ./utilities

EXTRA_COMPONENTS = \
	extras/http-parser \
	extras/pwm \
	$(DEMO_PATH)/components/common/wolfssl \
	$(DEMO_PATH)/components/common/cJSON \
	$(DEMO_PATH)/components/common/homekit

FLASH_SIZE ?= 32

EXTRA_CFLAGS += -I../.. -DHOMEKIT_SHORT_APPLE_UUIDS

include $(SDK_PATH)/common.mk

monitor:
	$(FILTEROUTPUT) --port $(ESPPORT) --baud 115200 --elf $(PROGRAM_OUT)