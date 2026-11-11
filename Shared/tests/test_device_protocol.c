/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include <stdio.h>
#include "device_protocol.h"

static int expect(int condition, const char *name)
{
	if (!condition) {
		(void)fprintf(stderr, "FAIL: %s\n", name);
		return 0;
	}
	(void)printf("PASS: %s\n", name);
	return 1;
}

static int test_ping(void)
{
	const dpc_u8 request[] = {1, DPC_OPERATION_PING, 0x34, 0x12, 0, 0};
	const dpc_u8 expected[] = {1, DPC_RESPONSE_OK, 0x34, 0x12, 0, 0};
	dpc_u8 response[sizeof(expected)];
	dpc_size_t response_length = 0;
	dpc_core_status status = dpc_process_message(request, sizeof(request), response, sizeof(response), &response_length);
	dpc_size_t index;

	if (status != DPC_CORE_SUCCESS || response_length != sizeof(expected)) {
		return 0;
	}
	for (index = 0; index < response_length; ++index) {
		if (response[index] != expected[index]) {
			return 0;
		}
	}
	return 1;
}

static int test_echo(void)
{
	const dpc_u8 request[] = {1, DPC_OPERATION_ECHO, 7, 0, 3, 0, 0x00, 0x7f, 0xff};
	const dpc_u8 expected[] = {1, DPC_RESPONSE_OK, 7, 0, 3, 0, 0x00, 0x7f, 0xff};
	dpc_u8 response[sizeof(expected)];
	dpc_size_t response_length = 0;
	dpc_core_status status = dpc_process_message(request, sizeof(request), response, sizeof(response), &response_length);
	dpc_size_t index;

	if (status != DPC_CORE_SUCCESS || response_length != sizeof(expected)) {
		return 0;
	}
	for (index = 0; index < response_length; ++index) {
		if (response[index] != expected[index]) {
			return 0;
		}
	}
	return 1;
}

static int test_unknown_operation(void)
{
	const dpc_u8 request[] = {1, 0xff, 9, 0, 0, 0};
	dpc_u8 response[DPC_HEADER_SIZE];
	dpc_size_t response_length = 0;
	dpc_core_status status = dpc_process_message(request, sizeof(request), response, sizeof(response), &response_length);
	return status == DPC_CORE_SUCCESS && response_length == DPC_HEADER_SIZE &&
		response[1] == DPC_RESPONSE_NOT_SUPPORTED && response[2] == 9;
}

static int test_rejects_bad_length(void)
{
	const dpc_u8 request[] = {1, DPC_OPERATION_ECHO, 1, 0, 2, 0, 0xaa};
	dpc_u8 response[DPC_HEADER_SIZE];
	dpc_size_t response_length = 99;
	dpc_core_status status = dpc_process_message(request, sizeof(request), response, sizeof(response), &response_length);
	return status == DPC_CORE_INVALID_MESSAGE && response_length == 0;
}

static int test_capacity_check(void)
{
	const dpc_u8 request[] = {1, DPC_OPERATION_ECHO, 1, 0, 1, 0, 0xaa};
	dpc_u8 response[DPC_HEADER_SIZE];
	dpc_size_t response_length = 0;
	dpc_core_status status = dpc_process_message(request, sizeof(request), response, sizeof(response), &response_length);
	return status == DPC_CORE_BUFFER_TOO_SMALL && response_length == DPC_HEADER_SIZE + 1;
}

int main(void)
{
	int passed = 0;
	int total = 5;
	passed += expect(test_ping(), "ping round trip");
	passed += expect(test_echo(), "echo round trip");
	passed += expect(test_unknown_operation(), "unknown operation response");
	passed += expect(test_rejects_bad_length(), "reject malformed payload length");
	passed += expect(test_capacity_check(), "check response capacity before writing");
	(void)printf("%d/%d tests passed\n", passed, total);
	return passed == total ? 0 : 1;
}
