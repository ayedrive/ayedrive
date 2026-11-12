// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
#include "device_protocol.h"

int main()
{
	const dpc_u8 request[] = {DPC_PROTOCOL_VERSION, DPC_OPERATION_PING, 1, 0, 0, 0};
	const dpc_u8 expected[] = {DPC_PROTOCOL_VERSION, DPC_RESPONSE_OK, 1, 0, 0, 0};
	dpc_u8 response[DPC_HEADER_SIZE] = {};
	dpc_size_t response_length = 0;

	if (dpc_process_message(request, sizeof(request), response, sizeof(response), &response_length) != DPC_CORE_SUCCESS) {
		return 1;
	}
	if (response_length != sizeof(expected)) {
		return 1;
	}
	for (dpc_size_t index = 0; index < response_length; ++index) {
		if (response[index] != expected[index]) {
			return 1;
		}
	}
	return 0;
}
