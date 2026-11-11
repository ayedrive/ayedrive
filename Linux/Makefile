# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
CC ?= cc
CPPFLAGS ?= -Iinclude -I../Shared/include
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -Werror -O2
BUILD_DIR ?= build
KDIR ?= /lib/modules/$(shell uname -r)/build

.PHONY: all user-client host-test kernel clean

all: user-client

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/test_protocol: user/test_protocol.c include/dpc_linux_uapi.h ../Shared/include/device_protocol.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

user-client: $(BUILD_DIR)/test_protocol

host-test:
	$(MAKE) -C ../Shared test

kernel:
	$(MAKE) -C $(KDIR) M=$(CURDIR)/kernel modules

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C ../Shared clean
