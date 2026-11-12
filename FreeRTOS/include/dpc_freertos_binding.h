/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DPC_FREERTOS_BINDING_H
#define DPC_FREERTOS_BINDING_H

#include "device_protocol.h"

typedef void (*dpc_freertos_completion)(
	void *context,
	dpc_core_status status,
	dpc_size_t response_length);

typedef struct dpc_freertos_request {
	const dpc_u8 *input;
	dpc_size_t input_length;
	dpc_u8 *output;
	dpc_size_t output_capacity;
	dpc_freertos_completion completion;
	void *completion_context;
} dpc_freertos_request;

/* Buffers remain caller-owned until the completion callback runs. */
int dpc_freertos_start(void);
int dpc_freertos_submit(const dpc_freertos_request *request);

#endif
