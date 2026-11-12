# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
CC ?= cc
CXX ?= c++
CPPFLAGS ?= -Iinclude
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -Werror -O2
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2
BUILD_DIR ?= build

.PHONY: all test test-cpp test-store test-sim test-virtual-model test-vm-guest-probe clean

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

test-store: $(BUILD_DIR)/test_device_store
	./$(BUILD_DIR)/test_device_store

test-sim: $(BUILD_DIR)/test_device_protocol_sim
	./$(BUILD_DIR)/test_device_protocol_sim

test-virtual-model: $(BUILD_DIR)/test_device_virtual_model
	./$(BUILD_DIR)/test_device_virtual_model

test-vm-guest-probe: $(BUILD_DIR)/test_vm_guest_probe
	./$(BUILD_DIR)/test_vm_guest_probe

test: $(BUILD_DIR)/test_device_protocol $(BUILD_DIR)/test_device_protocol_cpp $(BUILD_DIR)/test_device_store $(BUILD_DIR)/test_device_protocol_sim $(BUILD_DIR)/test_device_virtual_model $(BUILD_DIR)/test_vm_guest_probe
	./$(BUILD_DIR)/test_device_protocol
	./$(BUILD_DIR)/test_device_protocol_cpp
	./$(BUILD_DIR)/test_device_store
	./$(BUILD_DIR)/test_device_protocol_sim
	./$(BUILD_DIR)/test_device_virtual_model
	./$(BUILD_DIR)/test_vm_guest_probe

clean:
	rm -rf $(BUILD_DIR)
