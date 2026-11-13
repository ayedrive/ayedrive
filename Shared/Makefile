# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
CC ?= cc
CXX ?= c++
CPPFLAGS ?= -Iinclude
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -Werror -O2
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2
BUILD_DIR ?= build

.PHONY: all test test-cpp test-store test-sim test-virtual-model test-vm-guest-probe test-display-sim test-display-linux-headless display-linux-x11 test-display-transport display-web-source clean

all: $(BUILD_DIR)/test_device_protocol

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/test_device_protocol: core/device_protocol.c tests/test_device_protocol.c include/device_protocol.h include/device_types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) core/device_protocol.c tests/test_device_protocol.c -o $@

$(BUILD_DIR)/device_protocol.o: core/device_protocol.c include/device_protocol.h include/device_types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_device_protocol_cpp: tests/test_device_protocol_cpp.cpp $(BUILD_DIR)/device_protocol.o include/device_protocol.h | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests/test_device_protocol_cpp.cpp $(BUILD_DIR)/device_protocol.o -o $@

test-cpp: $(BUILD_DIR)/test_device_protocol_cpp
	./$(BUILD_DIR)/test_device_protocol_cpp

$(BUILD_DIR)/test_device_store: core/device_store.c tests/test_device_store.c include/device_store.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c11 core/device_store.c tests/test_device_store.c -o $@

$(BUILD_DIR)/test_device_protocol_sim: core/device_protocol.c core/device_protocol_sim.c tests/test_device_protocol_sim.c include/device_protocol.h include/device_protocol_sim.h include/device_hal.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c11 core/device_protocol.c core/device_protocol_sim.c tests/test_device_protocol_sim.c -o $@

$(BUILD_DIR)/test_device_virtual_model: core/device_virtual_model.c tests/test_device_virtual_model.c include/device_virtual_model.h include/device_hal.h include/device_types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c11 core/device_virtual_model.c tests/test_device_virtual_model.c -o $@

$(BUILD_DIR)/test_vm_guest_probe: core/device_virtual_model.c tests/test_vm_guest_probe.c include/device_virtual_model.h include/device_hal.h include/device_types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c11 core/device_virtual_model.c tests/test_vm_guest_probe.c -o $@

$(BUILD_DIR)/test_display_sim: core/display_sim.c tests/test_display_sim.c include/display_sim.h include/device_hal.h include/device_types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c11 core/display_sim.c tests/test_display_sim.c -o $@

test-store: $(BUILD_DIR)/test_device_store
	./$(BUILD_DIR)/test_device_store

test-sim: $(BUILD_DIR)/test_device_protocol_sim
	./$(BUILD_DIR)/test_device_protocol_sim

test-virtual-model: $(BUILD_DIR)/test_device_virtual_model
	./$(BUILD_DIR)/test_device_virtual_model

test-vm-guest-probe: $(BUILD_DIR)/test_vm_guest_probe
	./$(BUILD_DIR)/test_vm_guest_probe

test-display-sim: $(BUILD_DIR)/test_display_sim
	./$(BUILD_DIR)/test_display_sim

$(BUILD_DIR)/display_linux_demo_headless: core/display_sim.c tests/display_linux_demo.c include/display_sim.h include/device_hal.h include/device_types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c11 core/display_sim.c tests/display_linux_demo.c -o $@

test-display-linux-headless: $(BUILD_DIR)/display_linux_demo_headless
	./$(BUILD_DIR)/display_linux_demo_headless --headless

$(BUILD_DIR)/display_web_source: core/display_sim.c tests/display_web_source.c include/display_sim.h include/display_transport.h include/device_hal.h include/device_types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c11 core/display_sim.c tests/display_web_source.c -o $@

display-web-source: $(BUILD_DIR)/display_web_source

$(BUILD_DIR)/test_display_transport: tests/display_transport.c tests/test_display_transport.c include/display_transport.h include/device_types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c11 tests/display_transport.c tests/test_display_transport.c -o $@

test-display-transport: $(BUILD_DIR)/test_display_transport
	./$(BUILD_DIR)/test_display_transport

display-linux-x11: core/display_sim.c tests/display_linux_demo.c include/display_sim.h include/device_hal.h include/device_types.h | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -std=c11 -DDPC_DISPLAY_X11 core/display_sim.c tests/display_linux_demo.c -lX11 -o $(BUILD_DIR)/display_linux_demo_x11
	./$(BUILD_DIR)/display_linux_demo_x11

test: $(BUILD_DIR)/test_device_protocol $(BUILD_DIR)/test_device_protocol_cpp $(BUILD_DIR)/test_device_store $(BUILD_DIR)/test_device_protocol_sim $(BUILD_DIR)/test_device_virtual_model $(BUILD_DIR)/test_vm_guest_probe $(BUILD_DIR)/test_display_sim
	./$(BUILD_DIR)/test_device_protocol
	./$(BUILD_DIR)/test_device_protocol_cpp
	./$(BUILD_DIR)/test_device_store
	./$(BUILD_DIR)/test_device_protocol_sim
	./$(BUILD_DIR)/test_device_virtual_model
	./$(BUILD_DIR)/test_vm_guest_probe
	./$(BUILD_DIR)/test_display_sim

clean:
	rm -rf $(BUILD_DIR)
