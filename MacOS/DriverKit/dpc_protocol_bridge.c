/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "dpc_protocol_bridge.h"

/* Called by a target-specific DriverKit external-method implementation. */
dpc_core_status dpc_macos_process_message(
	const dpc_u8 *request,
	dpc_size_t request_length,
	dpc_u8 *response,
	dpc_size_t response_capacity,
	dpc_size_t *response_length)
{
	return dpc_process_message(request, request_length, response, response_capacity, response_length);
}
