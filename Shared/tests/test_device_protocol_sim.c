/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "device_protocol_sim.h"

#include "device_protocol.h"

#include <stdio.h>
#include <string.h>

typedef struct completion_result {
	dpc_u32 calls;
	dpc_hal_status status;
	dpc_size_t bytes;
} completion_result;

static void complete(void *context, dpc_hal_status status, dpc_size_t bytes)
{
	completion_result *result = (completion_result *)context;
	result->calls += 1U;
	result->status = status;
	result->bytes = bytes;
}

static int expect(int condition, const char *name)
{
	if (!condition) {
		(void)fprintf(stderr, "FAIL: %s\n", name);
		return 0;
	}
	(void)printf("PASS: %s\n", name);
	return 1;
}

static int test_ping_round_trip_and_virtual_time(void)
{
	dpc_protocol_sim simulator = { 0 };
	dpc_hal_ops operations = { 0 };
	dpc_hal_request request = { 0 };
	completion_result completion = { 0U, DPC_HAL_INVALID_REQUEST, 0U };
	const dpc_u8 input[DPC_HEADER_SIZE] = { DPC_PROTOCOL_VERSION, DPC_OPERATION_PING, 1U, 0U, 0U, 0U };
	dpc_u8 output[DPC_HEADER_SIZE] = { 0U };
	int success = 0;

	dpc_protocol_sim_init(&simulator, &operations, UINT64_C(250));
	request.operation = DPC_HAL_CONTROL_TRANSFER;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	if (operations.start(operations.context) == DPC_HAL_ACCEPTED) {
		success = (operations.submit(operations.context, &request, complete, &completion) == DPC_HAL_ACCEPTED) &&
				  (completion.calls == 1U) && (completion.status == DPC_HAL_ACCEPTED) &&
				  (completion.bytes == DPC_HEADER_SIZE) && (output[1] == DPC_RESPONSE_OK) &&
				  (simulator.transfer_count == 1U) && (operations.monotonic_time_ns(operations.context) == UINT64_C(250));
	}
	return success;
}

static int test_fault_is_one_shot(void)
{
	dpc_protocol_sim simulator = { 0 };
	dpc_hal_ops operations = { 0 };
	dpc_hal_request request = { 0 };
	completion_result completion = { 0U, DPC_HAL_INVALID_REQUEST, 0U };
	const dpc_u8 input[DPC_HEADER_SIZE] = { DPC_PROTOCOL_VERSION, DPC_OPERATION_PING, 2U, 0U, 0U, 0U };
	dpc_u8 output[DPC_HEADER_SIZE] = { 0U };
	int success = 0;

	dpc_protocol_sim_init(&simulator, &operations, UINT64_C(10));
	request.operation = DPC_HAL_BULK_TRANSFER;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	if (operations.start(operations.context) == DPC_HAL_ACCEPTED) {
		dpc_protocol_sim_inject_fault(&simulator, DPC_SIM_FAULT_NEXT_TRANSFER);
		success = (operations.submit(operations.context, &request, complete, &completion) == DPC_HAL_DEVICE_ERROR) &&
				  (completion.calls == 1U) && (simulator.transfer_count == 0U) &&
				  (operations.submit(operations.context, &request, complete, &completion) == DPC_HAL_ACCEPTED) &&
				  (completion.calls == 2U) && (simulator.transfer_count == 1U);
	}
	return success;
}

static int test_timeout_and_unsupported_operation(void)
{
	dpc_protocol_sim simulator = { 0 };
	dpc_hal_ops operations = { 0 };
	dpc_hal_request request = { 0 };
	completion_result completion = { 0U, DPC_HAL_INVALID_REQUEST, 0U };
	const dpc_u8 input[DPC_HEADER_SIZE] = { DPC_PROTOCOL_VERSION, DPC_OPERATION_PING, 3U, 0U, 0U, 0U };
	dpc_u8 output[DPC_HEADER_SIZE] = { 0U };
	int success = 0;

	dpc_protocol_sim_init(&simulator, &operations, UINT64_C(1000001));
	request.operation = DPC_HAL_INTERRUPT_TRANSFER;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	request.timeout_ms = 1U;
	if (operations.start(operations.context) == DPC_HAL_ACCEPTED) {
		success = (operations.submit(operations.context, &request, complete, &completion) == DPC_HAL_DEVICE_ERROR) &&
				  (completion.calls == 1U) && (simulator.transfer_count == 0U);
		request.timeout_ms = 0U;
		request.operation = DPC_HAL_REGISTER_READ;
		success = success && (operations.submit(operations.context, &request, complete, &completion) == DPC_HAL_NOT_SUPPORTED) &&
				  (completion.calls == 2U);
	}
	return success;
}

static int test_lifecycle_and_clock_overflow(void)
{
	dpc_protocol_sim simulator = { 0 };
	dpc_hal_ops operations = { 0 };
	dpc_hal_request request = { 0 };
	completion_result completion = { 0U, DPC_HAL_INVALID_REQUEST, 0U };
	const dpc_u8 input[DPC_HEADER_SIZE] = { DPC_PROTOCOL_VERSION, DPC_OPERATION_PING, 4U, 0U, 0U, 0U };
	dpc_u8 output[DPC_HEADER_SIZE] = { 0U };
	int success;

	dpc_protocol_sim_init(&simulator, &operations, 0U);
	request.operation = DPC_HAL_CONTROL_TRANSFER;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	success = (operations.submit(operations.context, &request, complete, &completion) == DPC_HAL_DEVICE_ERROR) &&
			  (completion.calls == 1U) && (completion.status == DPC_HAL_DEVICE_ERROR) &&
			  (operations.reset(operations.context) == DPC_HAL_ACCEPTED) &&
			  (operations.start(operations.context) == DPC_HAL_ACCEPTED) &&
			  (dpc_protocol_sim_advance(&simulator, UINT64_MAX) == DPC_HAL_ACCEPTED) &&
			  (dpc_protocol_sim_advance(&simulator, 1U) == DPC_HAL_DEVICE_ERROR);
	operations.stop(operations.context);
	return success && (simulator.started == 0U);
}

int main(void)
{
	int passed = 0;
	const int total = 4;
	passed += expect(test_ping_round_trip_and_virtual_time(), "HAL-backed PING and deterministic clock");
	passed += expect(test_fault_is_one_shot(), "one-shot injected transport failure");
	passed += expect(test_timeout_and_unsupported_operation(), "timeout and unsupported operation status");
	passed += expect(test_lifecycle_and_clock_overflow(), "lifecycle and virtual-clock overflow");
	(void)printf("%d/%d simulator tests passed\n", passed, total);
	return (passed == total) ? 0 : 1;
}
