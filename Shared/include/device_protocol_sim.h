/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DEVICE_PROTOCOL_SIM_H
#define DEVICE_PROTOCOL_SIM_H

#include "device_hal.h"

#define DPC_SIM_MAX_FRAME_SIZE (DPC_SIM_HEADER_SIZE + DPC_SIM_MAX_PAYLOAD_SIZE)
#define DPC_SIM_HEADER_SIZE 6U
#define DPC_SIM_MAX_PAYLOAD_SIZE 1024U

typedef enum dpc_sim_fault {
	DPC_SIM_FAULT_NONE = 0,
	DPC_SIM_FAULT_NEXT_TRANSFER = 1
} dpc_sim_fault;

typedef struct dpc_protocol_sim {
	dpc_u64 now_ns;
	dpc_u64 transfer_latency_ns;
	dpc_u32 started;
	dpc_u32 transfer_count;
	dpc_sim_fault fault;
} dpc_protocol_sim;

void dpc_protocol_sim_init(dpc_protocol_sim *simulator, dpc_hal_ops *operations, dpc_u64 transfer_latency_ns);
/* Transfers complete synchronously; submit invokes the completion callback inline exactly once. */
dpc_hal_status dpc_protocol_sim_advance(dpc_protocol_sim *simulator, dpc_u64 nanoseconds);
void dpc_protocol_sim_inject_fault(dpc_protocol_sim *simulator, dpc_sim_fault fault);

#endif
