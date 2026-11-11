/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DPC_MACOS_PROTOCOL_BRIDGE_H
#define DPC_MACOS_PROTOCOL_BRIDGE_H

#include "device_protocol.h"

dpc_core_status dpc_macos_process_message(
	const dpc_u8 *request,
	dpc_size_t request_length,
	dpc_u8 *response,
	dpc_size_t response_capacity,
	dpc_size_t *response_length);

#endif
