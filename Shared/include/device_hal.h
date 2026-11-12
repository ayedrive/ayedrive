/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DEVICE_HAL_H
#define DEVICE_HAL_H

#include "device_types.h"

#define DPC_HAL_ABI_VERSION 1u

typedef enum dpc_hal_operation {
	DPC_HAL_CONTROL_TRANSFER = 1,
	DPC_HAL_BULK_TRANSFER = 2,
	DPC_HAL_INTERRUPT_TRANSFER = 3,
	DPC_HAL_REGISTER_READ = 4,
	DPC_HAL_REGISTER_WRITE = 5,
	DPC_HAL_BLOCK_READ = 6,
	DPC_HAL_BLOCK_WRITE = 7,
	DPC_HAL_PACKET_SEND = 8,
	DPC_HAL_PACKET_RECEIVE = 9,
	DPC_HAL_STREAM_READ = 10,
	DPC_HAL_DMA_READ = 11,
	DPC_HAL_DMA_WRITE = 12
} dpc_hal_operation;

typedef enum dpc_hal_status {
	DPC_HAL_ACCEPTED = 0,
	DPC_HAL_NOT_SUPPORTED = -1,
	DPC_HAL_INVALID_REQUEST = -2,
	DPC_HAL_DEVICE_ERROR = -3,
	DPC_HAL_CANCELLED = -4
} dpc_hal_status;

typedef struct dpc_hal_request {
	dpc_u32 request_id;
	dpc_u32 operation;
	dpc_u64 address;
	const dpc_u8 *input;
	dpc_size_t input_length;
	dpc_u8 *output;
	dpc_size_t output_capacity;
	dpc_size_t output_length;
	dpc_u32 timeout_ms;
} dpc_hal_request;

typedef void (*dpc_hal_completion)(
	void *completion_context,
	dpc_hal_status status,
	dpc_size_t bytes_transferred);

typedef struct dpc_hal_ops {
	dpc_u32 abi_version;
	dpc_u32 struct_size;
	void *context;
	dpc_hal_status (*start)(void *context);
	dpc_hal_status (*submit)(
		void *context,
		dpc_hal_request *request,
		dpc_hal_completion completion,
		void *completion_context);
	dpc_hal_status (*cancel)(void *context, dpc_u32 request_id);
	dpc_hal_status (*reset)(void *context);
	void (*stop)(void *context);
	dpc_u64 (*monotonic_time_ns)(void *context);
} dpc_hal_ops;

/* The request and its buffers remain owned by the caller until completion. */
#endif
