/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "display_transport.h"

#include <stddef.h>

int dpc_display_transport_valid(const dpc_display_transport_header *header)
{
	if (header == NULL || header->magic != DPC_DISPLAY_TRANSPORT_MAGIC ||
		header->version != DPC_DISPLAY_TRANSPORT_VERSION ||
		header->width == 0U || header->height == 0U ||
		header->payload_length != header->width * header->height * 4U ||
		header->payload_length > DPC_DISPLAY_TRANSPORT_MAX_FRAME) {
		return 0;
	}
	return 1;
}
