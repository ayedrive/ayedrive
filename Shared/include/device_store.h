/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DEVICE_STORE_H
#define DEVICE_STORE_H

#include <stdint.h>

#define DPC_STORE_CAPACITY 256U
#define DPC_STORE_COMMIT_MARKER UINT32_C(0xD0C0A55A)

typedef enum dpc_store_status {
	DPC_STORE_OK = 0,
	DPC_STORE_INVALID = -1,
	DPC_STORE_CORRUPT = -2,
	DPC_STORE_INTERRUPTED = -3,
	DPC_STORE_NO_VALID_BANK = -4
} dpc_store_status;

typedef enum dpc_store_cut_point {
	DPC_STORE_NO_CUT = 0,
	DPC_STORE_CUT_AFTER_INVALIDATE = 1,
	DPC_STORE_CUT_AFTER_DATA = 2,
	DPC_STORE_CUT_AFTER_CRC = 3,
	DPC_STORE_CUT_AFTER_MARKER = 4
} dpc_store_cut_point;

typedef struct dpc_store_bank {
	uint8_t bytes[DPC_STORE_CAPACITY];
	uint32_t length;
	uint32_t generation;
	uint32_t crc32;
	uint32_t commit_marker;
} dpc_store_bank;

typedef struct dpc_store {
	dpc_store_bank banks[2];
	uint32_t active_bank;
} dpc_store;

uint32_t dpc_store_crc32(const uint8_t *data, uint32_t length);
dpc_store_status dpc_store_format(dpc_store *store, const uint8_t *initial, uint32_t length);
dpc_store_status dpc_store_select(dpc_store *store);
dpc_store_status dpc_store_read(const dpc_store *store, uint32_t offset, uint8_t *output, uint32_t length);
dpc_store_status dpc_store_commit(dpc_store *store, uint32_t offset, const uint8_t *input, uint32_t length, dpc_store_cut_point cut_point);

#endif
