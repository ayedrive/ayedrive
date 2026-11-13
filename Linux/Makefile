# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
CC ?= cc
CPPFLAGS ?= -Iinclude -I../Shared/include
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -Werror -O2
BUILD_DIR ?= build
KDIR ?= /lib/modules/$(shell uname -r)/build

.PHONY: all user-client display-guest qemu-display-guest qemu-guest-image host-test kernel clean

all: user-client

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/test_protocol: user/test_protocol.c include/dpc_linux_uapi.h ../Shared/include/device_protocol.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@

user-client: $(BUILD_DIR)/test_protocol

$(BUILD_DIR)/display_guest: user/display_guest.c ../Shared/core/display_sim.c ../Shared/include/display_sim.h ../Shared/include/display_transport.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) -I../Shared/include $(CFLAGS) -std=c11 -static ../Shared/core/display_sim.c user/display_guest.c -o $@

display-guest: $(BUILD_DIR)/display_guest

qemu-display-guest: $(BUILD_DIR)/display_guest
	@printf '%s\n' 'Guest client built: $(BUILD_DIR)/display_guest; run it inside the QEMU guest as /dev/virtio-ports/ayeadrive.display.'

qemu-guest-image: $(BUILD_DIR)/display_guest
	bash ../tools/build_display_guest_initramfs.sh $(BUILD_DIR)/display_guest $(BUILD_DIR)/display_guest.initramfs

host-test:
	$(MAKE) -C ../Shared test

kernel:
	$(MAKE) -C $(KDIR) M=$(CURDIR)/kernel modules

clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C ../Shared clean
