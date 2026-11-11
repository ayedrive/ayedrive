/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "device_protocol.h"

static dpc_u16 dpc_read_u16_le(const dpc_u8 *bytes)
{
	return (dpc_u16)((dpc_u16)bytes[0] | ((dpc_u16)bytes[1] << 8));
}

static void dpc_write_u16_le(dpc_u8 *bytes, dpc_u16 value)
{
	bytes[0] = (dpc_u8)(value & 0xffu);
	bytes[1] = (dpc_u8)((value >> 8) & 0xffu);
}

static void dpc_copy_bytes(dpc_u8 *destination, const dpc_u8 *source, dpc_size_t length)
{
	dpc_size_t index;
	for (index = 0; index < length; ++index) {
		destination[index] = source[index];
	}
}

dpc_core_status dpc_process_message(
	const dpc_u8 *request,
	dpc_size_t request_length,
	dpc_u8 *response,
	dpc_size_t response_capacity,
	dpc_size_t *response_length)
{
	dpc_u16 request_id;
	dpc_u16 payload_length;
	dpc_u16 response_payload_length;
	dpc_response_status response_status;
	dpc_size_t required_length;

	if (response_length == NULL) {
		return DPC_CORE_INVALID_ARGUMENT;
	}
	*response_length = 0;

	if (request == NULL || response == NULL || request_length < DPC_HEADER_SIZE) {
		return DPC_CORE_INVALID_ARGUMENT;
	}
	if (request[0] != DPC_PROTOCOL_VERSION) {
		return DPC_CORE_UNSUPPORTED_VERSION;
	}

	request_id = dpc_read_u16_le(&request[2]);
	payload_length = dpc_read_u16_le(&request[4]);
	if (payload_length > DPC_MAX_PAYLOAD_SIZE ||
		(dpc_size_t)payload_length != request_length - DPC_HEADER_SIZE) {
		return DPC_CORE_INVALID_MESSAGE;
	}

	response_status = DPC_RESPONSE_OK;
	response_payload_length = 0;
	if (request[1] == DPC_OPERATION_PING) {
		if (payload_length != 0) {
			return DPC_CORE_INVALID_MESSAGE;
		}
	} else if (request[1] == DPC_OPERATION_ECHO) {
		response_payload_length = payload_length;
	} else {
		response_status = DPC_RESPONSE_NOT_SUPPORTED;
	}

	required_length = DPC_HEADER_SIZE + (dpc_size_t)response_payload_length;
	if (response_capacity < required_length) {
		*response_length = required_length;
		return DPC_CORE_BUFFER_TOO_SMALL;
	}

	response[0] = DPC_PROTOCOL_VERSION;
	response[1] = (dpc_u8)response_status;
	dpc_write_u16_le(&response[2], request_id);
	dpc_write_u16_le(&response[4], response_payload_length);
	if (response_payload_length != 0) {
		dpc_copy_bytes(&response[DPC_HEADER_SIZE], &request[DPC_HEADER_SIZE], response_payload_length);
	}
	*response_length = required_length;
	return DPC_CORE_SUCCESS;
}
