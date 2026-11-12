/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DEVICE_PROTOCOL_H
#define DEVICE_PROTOCOL_H

#include "device_types.h"

#define DPC_PROTOCOL_VERSION 1u
#define DPC_HEADER_SIZE 6u
#define DPC_MAX_PAYLOAD_SIZE 1024u

typedef enum dpc_operation {
	DPC_OPERATION_PING = 1,
	DPC_OPERATION_ECHO = 2
} dpc_operation;

typedef enum dpc_response_status {
	DPC_RESPONSE_OK = 0,
	DPC_RESPONSE_NOT_SUPPORTED = 1
} dpc_response_status;

typedef enum dpc_core_status {
	DPC_CORE_SUCCESS = 0,
	DPC_CORE_INVALID_ARGUMENT = -1,
	DPC_CORE_INVALID_MESSAGE = -2,
	DPC_CORE_UNSUPPORTED_VERSION = -3,
	DPC_CORE_BUFFER_TOO_SMALL = -4
} dpc_core_status;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Reference/test protocol only; it is not a claim about any physical device.
 * Request and response bytes are: version:u8, operation/status:u8,
 * request_id:u16-le, payload_length:u16-le, payload bytes.
 */
dpc_core_status dpc_process_message(
	const dpc_u8 *request,
	dpc_size_t request_length,
	dpc_u8 *response,
	dpc_size_t response_capacity,
	dpc_size_t *response_length);

#ifdef __cplusplus
}
#endif

#endif
