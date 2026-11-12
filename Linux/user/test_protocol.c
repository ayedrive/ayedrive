// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "dpc_linux_uapi.h"

int main(void)
{
	const dpc_u8 request[] = {DPC_PROTOCOL_VERSION, DPC_OPERATION_PING, 0x34, 0x12, 0, 0};
	const dpc_u8 expected[] = {DPC_PROTOCOL_VERSION, DPC_RESPONSE_OK, 0x34, 0x12, 0, 0};
	dpc_linux_exchange exchange;
	int descriptor;
	int result = 1;

	descriptor = open("/dev/dpc-reference", O_RDWR | O_CLOEXEC);
	if (descriptor < 0) {
		perror("open /dev/dpc-reference");
		return 2;
	}

	memset(&exchange, 0, sizeof(exchange));
	exchange.input_length = sizeof(request);
	exchange.output_capacity = DPC_LINUX_FRAME_CAPACITY;
	memcpy(exchange.input, request, sizeof(request));
	if (ioctl(descriptor, DPC_LINUX_IOCTL_EXCHANGE, &exchange) != 0) {
		perror("DPC_LINUX_IOCTL_EXCHANGE");
	} else if (exchange.output_length != sizeof(expected) ||
			   memcmp(exchange.output, expected, sizeof(expected)) != 0) {
		(void)fprintf(stderr, "unexpected protocol response\n");
	} else {
		(void)printf("PASS: Linux reference-device PING\n");
		result = 0;
	}

	(void)close(descriptor);
	return result;
}
