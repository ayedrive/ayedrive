/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DISPLAY_TRANSPORT_H
#define DISPLAY_TRANSPORT_H

#include "device_types.h"

#define DPC_DISPLAY_TRANSPORT_MAGIC 0x44504344u
#define DPC_DISPLAY_TRANSPORT_VERSION 1u
#define DPC_DISPLAY_TRANSPORT_HEADER_SIZE 24u
#define DPC_DISPLAY_TRANSPORT_MAX_WIDTH 4096u
#define DPC_DISPLAY_TRANSPORT_MAX_HEIGHT 2160u
#define DPC_DISPLAY_TRANSPORT_MAX_FRAME (DPC_DISPLAY_TRANSPORT_MAX_WIDTH * DPC_DISPLAY_TRANSPORT_MAX_HEIGHT * 4u)

typedef struct dpc_display_transport_header {
	dpc_u32 magic;
	dpc_u32 version;
	dpc_u32 width;
	dpc_u32 height;
	dpc_u32 payload_length;
	dpc_u32 frame_number;
} dpc_display_transport_header;

#endif
