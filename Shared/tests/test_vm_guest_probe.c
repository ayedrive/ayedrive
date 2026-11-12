/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "device_virtual_model.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
	dpc_virtual_model model = { 0 };
	dpc_hal_request request = { 0 };
	const dpc_u8 input[] = { 0xD1U, 0xD2U, 0xD3U, 0xD4U };
	dpc_u8 output[sizeof(input)] = { 0U };
	dpc_virtual_model_init(&model, DPC_VM_CHAR_CONTROL, 25U);
	request.operation = DPC_HAL_REGISTER_WRITE;
	request.input = input;
	request.input_length = sizeof(input);
	request.output = output;
	request.output_capacity = sizeof(output);
	if (dpc_virtual_model_start(&model) != DPC_HAL_ACCEPTED ||
		dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED) return 1;
	request.operation = DPC_HAL_REGISTER_READ;
	request.input_length = 0U;
	if (dpc_virtual_model_execute(&model, &request) != DPC_HAL_ACCEPTED ||
		request.output_length != sizeof(input) || memcmp(input, output, sizeof(input)) != 0) return 2;
	(void)printf("GUEST_SYNTHETIC_HAL_PROBE_PASSED virtual_time_ns=%llu\n", (unsigned long long)model.now_ns);
	return 0;
}
