/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "device_protocol_sim.h"

#include "device_protocol.h"

#include <limits.h>

static dpc_hal_status simulator_start(void *context)
{
	dpc_hal_status result = DPC_HAL_INVALID_REQUEST;
	if (context != NULL) {
		((dpc_protocol_sim *)context)->started = 1U;
		result = DPC_HAL_ACCEPTED;
	}
	return result;
}

static dpc_hal_status simulator_submit(
	void *context,
	dpc_hal_request *request,
	dpc_hal_completion completion,
	void *completion_context)
{
	dpc_protocol_sim *simulator = (dpc_protocol_sim *)context;
	dpc_hal_status result = DPC_HAL_INVALID_REQUEST;
	dpc_size_t response_length = 0U;
	dpc_core_status core_status = DPC_CORE_INVALID_ARGUMENT;

	if ((simulator != NULL) && (request != NULL) && (completion != NULL) &&
		(request->input != NULL) && (request->output != NULL)) {
		request->output_length = 0U;
		if (simulator->started == 0U) {
			result = DPC_HAL_DEVICE_ERROR;
		} else if (request->operation != DPC_HAL_CONTROL_TRANSFER &&
				   request->operation != DPC_HAL_BULK_TRANSFER &&
				   request->operation != DPC_HAL_INTERRUPT_TRANSFER) {
			result = DPC_HAL_NOT_SUPPORTED;
		} else if (simulator->fault == DPC_SIM_FAULT_NEXT_TRANSFER) {
			simulator->fault = DPC_SIM_FAULT_NONE;
			result = DPC_HAL_DEVICE_ERROR;
		} else if (simulator->transfer_latency_ns > (UINT64_MAX - simulator->now_ns)) {
			result = DPC_HAL_DEVICE_ERROR;
		} else if ((request->timeout_ms != 0U) &&
				   (simulator->transfer_latency_ns > ((dpc_u64)request->timeout_ms * UINT64_C(1000000)))) {
			result = DPC_HAL_DEVICE_ERROR;
		} else if (simulator->transfer_count == UINT32_MAX) {
			result = DPC_HAL_DEVICE_ERROR;
		} else {
			simulator->now_ns += simulator->transfer_latency_ns;
			simulator->transfer_count += 1U;
			core_status = dpc_process_message(
				request->input,
				request->input_length,
				request->output,
				request->output_capacity,
				&response_length);
			if (core_status == DPC_CORE_SUCCESS) {
				request->output_length = response_length;
				result = DPC_HAL_ACCEPTED;
			} else {
				result = DPC_HAL_INVALID_REQUEST;
			}
		}
		completion(completion_context, result, request->output_length);
	}
	return result;
}

static dpc_hal_status simulator_cancel(void *context, dpc_u32 request_id)
{
	dpc_hal_status result = DPC_HAL_INVALID_REQUEST;
	(void)request_id;
	if (context != NULL) {
		result = DPC_HAL_NOT_SUPPORTED;
	}
	return result;
}

static dpc_hal_status simulator_reset(void *context)
{
	dpc_hal_status result = DPC_HAL_INVALID_REQUEST;
	if (context != NULL) {
		dpc_protocol_sim *simulator = (dpc_protocol_sim *)context;
		simulator->transfer_count = 0U;
		simulator->fault = DPC_SIM_FAULT_NONE;
		result = DPC_HAL_ACCEPTED;
	}
	return result;
}

static void simulator_stop(void *context)
{
	if (context != NULL) {
		((dpc_protocol_sim *)context)->started = 0U;
	}
}

static dpc_u64 simulator_time(void *context)
{
	dpc_u64 result = 0U;
	if (context != NULL) {
		result = ((dpc_protocol_sim *)context)->now_ns;
	}
	return result;
}

void dpc_protocol_sim_init(dpc_protocol_sim *simulator, dpc_hal_ops *operations, dpc_u64 transfer_latency_ns)
{
	if ((simulator != NULL) && (operations != NULL)) {
		simulator->now_ns = 0U;
		simulator->transfer_latency_ns = transfer_latency_ns;
		simulator->started = 0U;
		simulator->transfer_count = 0U;
		simulator->fault = DPC_SIM_FAULT_NONE;
		operations->abi_version = DPC_HAL_ABI_VERSION;
		operations->struct_size = (dpc_u32)sizeof(*operations);
		operations->context = simulator;
		operations->start = simulator_start;
		operations->submit = simulator_submit;
		operations->cancel = simulator_cancel;
		operations->reset = simulator_reset;
		operations->stop = simulator_stop;
		operations->monotonic_time_ns = simulator_time;
	}
}

dpc_hal_status dpc_protocol_sim_advance(dpc_protocol_sim *simulator, dpc_u64 nanoseconds)
{
	dpc_hal_status result = DPC_HAL_INVALID_REQUEST;
	if (simulator != NULL) {
		if (nanoseconds <= (UINT64_MAX - simulator->now_ns)) {
			simulator->now_ns += nanoseconds;
			result = DPC_HAL_ACCEPTED;
		} else {
			result = DPC_HAL_DEVICE_ERROR;
		}
	}
	return result;
}

void dpc_protocol_sim_inject_fault(dpc_protocol_sim *simulator, dpc_sim_fault fault)
{
	if ((simulator != NULL) &&
		(fault == DPC_SIM_FAULT_NONE || fault == DPC_SIM_FAULT_NEXT_TRANSFER)) {
		simulator->fault = fault;
	}
}
