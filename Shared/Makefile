# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
CC ?= cc
CXX ?= c++
CPPFLAGS ?= -Iinclude
CFLAGS ?= -std=c99 -Wall -Wextra -Wpedantic -Werror -O2
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2
BUILD_DIR ?= build

.PHONY: all test test-cpp clean

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

test: $(BUILD_DIR)/test_device_protocol $(BUILD_DIR)/test_device_protocol_cpp
	./$(BUILD_DIR)/test_device_protocol
	./$(BUILD_DIR)/test_device_protocol_cpp

clean:
	rm -rf $(BUILD_DIR)
