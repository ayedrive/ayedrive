/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DPC_LINUX_UAPI_H
#define DPC_LINUX_UAPI_H

#include "device_protocol.h"
#ifdef __KERNEL__
#include <linux/ioctl.h>
#else
#include <sys/ioctl.h>
#endif

#define DPC_LINUX_FRAME_CAPACITY (DPC_HEADER_SIZE + DPC_MAX_PAYLOAD_SIZE)
#define DPC_LINUX_IOC_MAGIC 'D'

typedef struct dpc_linux_exchange {
	dpc_u32 input_length;
	dpc_u32 output_capacity;
	dpc_u32 output_length;
	dpc_u8 input[DPC_LINUX_FRAME_CAPACITY];
	dpc_u8 output[DPC_LINUX_FRAME_CAPACITY];
} dpc_linux_exchange;

#define DPC_LINUX_IOCTL_EXCHANGE _IOWR(DPC_LINUX_IOC_MAGIC, 1, dpc_linux_exchange)

#endif
