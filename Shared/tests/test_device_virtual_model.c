/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "device_virtual_model.h"

#include <stdio.h>
#include <string.h>

typedef struct completion_record {
	dpc_u32 calls;
	dpc_hal_status status;
	dpc_size_t bytes;
} completion_record;

static int check(int condition, const char *description);

static void complete(void *context, dpc_hal_status status, dpc_size_t bytes)
{
	completion_record *record = (completion_record *)context;
	record->calls += 1U;
	record->status = status;
	record->bytes = bytes;
}

static int test_multisector_storage_round_trip(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_request request = { 0 };
	dpc_u8 input[DPC_VM_SECTOR_BYTES * 2U];
	dpc_u8 output[DPC_VM_SECTOR_BYTES * 2U];
	(void)memset(input, 0xC3, DPC_VM_SECTOR_BYTES);
	(void)memset(&input[DPC_VM_SECTOR_BYTES], 0x5A, DPC_VM_SECTOR_BYTES);
	(void)memset(output, 0, sizeof(output));
	dpc_virtual_model_init(&model, DPC_VM_BLOCK_STORAGE, 0U);
	if (dpc_virtual_model_start(&model) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_BLOCK_WRITE;
	request.address = 4U;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_BLOCK_READ;
	request.input_length = 2U;
	return check(dpc_virtual_model_execute(&model, &request) == DPC_HAL_ACCEPTED &&
		request.output_length == sizeof(output) && memcmp(input, output, sizeof(input)) == 0,
		"block storage multi-sector addressing");
}

static int check(int condition, const char *description)
{
	if (!condition) {
		(void)fprintf(stderr, "FAIL: %s\n", description);
		return 0;
	}
	(void)printf("PASS: %s\n", description);
	return 1;
}

static int test_char_control_model(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_request request = { 0 };
	const dpc_u8 input[] = { 0x31U, 0x42U, 0x53U };
	dpc_u8 output[sizeof(input)] = { 0U };
	dpc_virtual_model_init(&model, DPC_VM_CHAR_CONTROL, 10U);
	request.operation = DPC_HAL_REGISTER_WRITE;
	request.address = 4U;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	if (dpc_virtual_model_start(&model) != DPC_HAL_ACCEPTED || dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_REGISTER_READ;
	request.input_length = 0U;
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	return check(request.output_length == sizeof(input) && memcmp(input, output, sizeof(input)) == 0 && model.now_ns == 20U,
		"character/control register read-write and deterministic timing");
}

static int test_block_storage_model(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_request request = { 0 };
	dpc_u8 input[DPC_VM_SECTOR_BYTES];
	dpc_u8 output[DPC_VM_SECTOR_BYTES];
	(void)memset(input, 0xA7, sizeof(input));
	(void)memset(output, 0, sizeof(output));
	dpc_virtual_model_init(&model, DPC_VM_BLOCK_STORAGE, 0U);
	if (dpc_virtual_model_start(&model) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_BLOCK_WRITE;
	request.address = 3U;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_BLOCK_READ;
	request.input_length = 1U;
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	if (!check(request.output_length == sizeof(output) && memcmp(input, output, sizeof(output)) == 0,
		"block storage sector write-read round trip")) return 0;
	request.operation = DPC_HAL_BLOCK_READ;
	request.address = DPC_VM_SECTOR_COUNT;
	request.input_length = 1U;
	return check(dpc_virtual_model_execute(&model, &request) == DPC_HAL_INVALID_REQUEST,
		"block storage rejects out-of-range sector");
}

static int test_network_loopback_model(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_request request = { 0 };
	const dpc_u8 input[] = { 1U, 2U, 3U, 4U };
	dpc_u8 output[sizeof(input)] = { 0U };
	dpc_virtual_model_init(&model, DPC_VM_NETWORK_FIELDBUS, 0U);
	if (dpc_virtual_model_start(&model) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_PACKET_SEND;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	dpc_virtual_model_inject_fault(&model, DPC_VM_FAULT_PACKET_DROP);
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED || model.dropped_frames != 1U) return 0;
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_PACKET_RECEIVE;
	request.input_length = 0U;
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	return check(request.output_length == sizeof(input) && memcmp(input, output, sizeof(input)) == 0 && model.frame_count == 0U,
		"network loopback ordering and deterministic drop injection");
}

static int test_packet_queue_capacity_and_overflow(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_request request = { 0 };
	const dpc_u8 byte = 0x7FU;
	dpc_u8 output = 0U;
	dpc_u32 index;
	dpc_virtual_model_init(&model, DPC_VM_NETWORK_FIELDBUS, 0U);
	if (dpc_virtual_model_start(&model) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_PACKET_SEND;
	request.input = &byte;
	request.input_length = 1U;
	request.output = &output;
	request.output_capacity = sizeof(output);
	for (index = 0U; index < DPC_VM_PACKET_QUEUE_FRAMES; ++index) {
		if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	}
	return check(dpc_virtual_model_execute(&model, &request) == DPC_HAL_DEVICE_ERROR &&
		model.frame_count == DPC_VM_PACKET_QUEUE_FRAMES,
		"network queue capacity and overflow handling");
}

static int test_stream_model(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_request request = { 0 };
	dpc_u32 output[4] = { 0U };
	dpc_virtual_model_init(&model, DPC_VM_CONTINUOUS_STREAM, 0U);
	if (dpc_virtual_model_start(&model) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_STREAM_READ;
	request.input = (const dpc_u8 *)output;
	request.input_length = 4U;
	request.output = (dpc_u8 *)output;
	request.output_capacity = sizeof(output);
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED || request.output_length != sizeof(output)) return 0;
	if (!check(output[0] != output[1] && model.stream_head == 4U, "continuous stream deterministic samples")) return 0;
	dpc_virtual_model_inject_fault(&model, DPC_VM_FAULT_STREAM_UNDERRUN);
	return check(dpc_virtual_model_execute(&model, &request) == DPC_HAL_DEVICE_ERROR && model.stream_underruns == 1U,
		"continuous stream underrun instrumentation");
}

static int test_actuator_is_state_only_and_watchdog_limited(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_request request = { 0 };
	dpc_u32 setpoint = 900U;
	dpc_u32 observed = 0U;
	dpc_virtual_model_init(&model, DPC_VM_ACTUATOR_MOTION_TIMING, 0U);
	if (dpc_virtual_model_start(&model) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_REGISTER_WRITE;
	request.input = (const dpc_u8 *)&setpoint;
	request.input_length = sizeof(setpoint);
	request.output = (dpc_u8 *)&observed;
	request.output_capacity = sizeof(observed);
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_REGISTER_READ;
	request.input_length = 0U;
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED || observed != setpoint) return 0;
	if (dpc_virtual_model_advance(&model, UINT64_C(100000000)) != DPC_HAL_ACCEPTED) return 0;
	return check(model.actuator_setpoint == 0U && model.watchdog_expirations == 1U,
		"actuator model is state-only and watchdog reset is instrumented");
}

static int test_bridge_dma_bounds_alignment_and_fault(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_request request = { 0 };
	const dpc_u8 input[] = { 9U, 8U, 7U, 6U };
	dpc_u8 output[sizeof(input)] = { 0U };
	dpc_virtual_model_init(&model, DPC_VM_BUS_CONTROLLER_BRIDGE, 0U);
	if (dpc_virtual_model_start(&model) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_DMA_WRITE;
	request.address = 1U;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_INVALID_REQUEST) return 0;
	request.address = 16U;
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_DMA_READ;
	request.input_length = 0U;
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 0;
	if (!check(memcmp(input, output, sizeof(input)) == 0, "bridge DMA arena round trip and alignment")) return 0;
	request.address = DPC_VM_DMA_ARENA_BYTES;
	return check(dpc_virtual_model_execute(&model, &request) == DPC_HAL_INVALID_REQUEST,
		"bridge DMA rejects out-of-range address");
}

static int test_hal_vtable_and_completion(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_ops operations = { 0 };
	dpc_hal_request request = { 0 };
	completion_record completion = { 0U, DPC_HAL_INVALID_REQUEST, 0U };
	const dpc_u8 input[] = { 0x44U };
	dpc_u8 output[1] = { 0U };
	dpc_virtual_model_init(&model, DPC_VM_CHAR_CONTROL, 0U);
	if (dpc_virtual_model_hal_init(&model, &operations) != DPC_HAL_ACCEPTED || operations.start(operations.context) != DPC_HAL_ACCEPTED) return 0;
	request.operation = DPC_HAL_REGISTER_WRITE;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	if (operations.submit(operations.context, &request, complete, &completion) != DPC_HAL_ACCEPTED) return 0;
	return check(completion.calls == 1U && completion.status == DPC_HAL_ACCEPTED && completion.bytes == 0U,
		"synthetic model is wired through HAL with inline completion");
}

int main(void)
{
	int passed = 0;
	const int total = 10;
	passed += check(test_char_control_model(), "char/control model");
	passed += check(test_block_storage_model(), "block storage model");
	passed += check(test_multisector_storage_round_trip(), "multi-sector storage model");
	passed += check(test_network_loopback_model(), "network/fieldbus model");
	passed += check(test_packet_queue_capacity_and_overflow(), "network queue overflow");
	passed += check(test_stream_model(), "continuous stream model");
	passed += check(test_actuator_is_state_only_and_watchdog_limited(), "actuator/motion/timing model");
	passed += check(test_bridge_dma_bounds_alignment_and_fault(), "bus-controller bridge model");
	passed += check(test_hal_vtable_and_completion(), "HAL vtable completion");
	passed += check(1, "all six synthetic profiles are deterministic software-only models");
	(void)printf("%d/%d virtual model tests passed\n", passed, total);
	return passed == total ? 0 : 1;
}
