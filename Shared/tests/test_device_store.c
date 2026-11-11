/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "device_store.h"

#include <stdio.h>
#include <string.h>

static int expect(int condition, const char *name)
{
	if (!condition) {
		(void)fprintf(stderr, "FAIL: %s\n", name);
		return 0;
	}
	(void)printf("PASS: %s\n", name);
	return 1;
}

static int test_crc_vector(void)
{
	static const uint8_t input[] = "123456789";
	return dpc_store_crc32(input, 9U) == UINT32_C(0xCBF43926);
}

static int test_commit_and_read(void)
{
	dpc_store store = { 0 };
	const uint8_t initial[] = { 1U, 2U, 3U };
	const uint8_t update[] = { 9U, 8U };
	uint8_t output[3] = { 0U, 0U, 0U };
	int success = (dpc_store_format(&store, initial, 3U) == DPC_STORE_OK) &&
				  (dpc_store_commit(&store, 1U, update, 2U, DPC_STORE_NO_CUT) == DPC_STORE_OK) &&
				  (dpc_store_read(&store, 0U, output, 3U) == DPC_STORE_OK) &&
				  (output[0] == 1U) && (output[1] == 9U) && (output[2] == 8U) &&
				  (store.banks[store.active_bank].generation == 1U);
	return success;
}

static int test_interruption_preserves_old_or_complete_new_bank(void)
{
	const dpc_store_cut_point cuts[] = {
		DPC_STORE_CUT_AFTER_INVALIDATE,
		DPC_STORE_CUT_AFTER_DATA,
		DPC_STORE_CUT_AFTER_CRC,
		DPC_STORE_CUT_AFTER_MARKER
	};
	const uint8_t initial[] = { 4U, 5U, 6U };
	const uint8_t update[] = { 7U, 8U };
	uint8_t output[3] = { 0U, 0U, 0U };
	uint32_t index = 0U;
	int success = 1;

	for (index = 0U; (index < (uint32_t)(sizeof(cuts) / sizeof(cuts[0]))) && success; ++index) {
		dpc_store store = { 0 };
		success = (dpc_store_format(&store, initial, 3U) == DPC_STORE_OK) &&
				  (dpc_store_commit(&store, 1U, update, 2U, cuts[index]) == DPC_STORE_INTERRUPTED) &&
				  (dpc_store_select(&store) == DPC_STORE_OK) &&
				  (dpc_store_read(&store, 0U, output, 3U) == DPC_STORE_OK);
		if (success && (cuts[index] == DPC_STORE_CUT_AFTER_MARKER)) {
			success = (output[0] == 4U) && (output[1] == 7U) && (output[2] == 8U);
		} else if (success) {
			success = (output[0] == 4U) && (output[1] == 5U) && (output[2] == 6U);
		}
	}
	return success;
}

static int test_corrupt_new_bank_falls_back(void)
{
	dpc_store store = { 0 };
	const uint8_t initial[] = { 3U, 2U, 1U };
	const uint8_t update[] = { 9U };
	uint8_t output[3] = { 0U, 0U, 0U };
	uint32_t new_bank = 0U;
	int success = (dpc_store_format(&store, initial, 3U) == DPC_STORE_OK) &&
				  (dpc_store_commit(&store, 1U, update, 1U, DPC_STORE_NO_CUT) == DPC_STORE_OK);

	if (success) {
		new_bank = store.active_bank;
		store.banks[new_bank].bytes[1] ^= 0x01U;
		success = (dpc_store_select(&store) == DPC_STORE_OK) &&
				  (store.active_bank != new_bank) &&
				  (dpc_store_read(&store, 0U, output, 3U) == DPC_STORE_OK) &&
				  (memcmp(output, initial, sizeof(initial)) == 0);
	}
	return success;
}

static int test_bounds_and_empty_write(void)
{
	dpc_store store = { 0 };
	int success = (dpc_store_format(&store, NULL, 0U) == DPC_STORE_OK) &&
				  (dpc_store_commit(&store, DPC_STORE_CAPACITY, NULL, 0U, DPC_STORE_NO_CUT) == DPC_STORE_OK) &&
				  (dpc_store_commit(&store, DPC_STORE_CAPACITY, (const uint8_t *)"x", 1U, DPC_STORE_NO_CUT) == DPC_STORE_INVALID);
	return success;
}

int main(void)
{
	int passed = 0;
	const int total = 5;
	passed += expect(test_crc_vector(), "CRC32 standard check vector");
	passed += expect(test_commit_and_read(), "transactional write and read");
	passed += expect(test_interruption_preserves_old_or_complete_new_bank(), "interrupt at each commit stage");
	passed += expect(test_corrupt_new_bank_falls_back(), "corrupt bank selection falls back");
	passed += expect(test_bounds_and_empty_write(), "bounds and empty update");
	(void)printf("%d/%d store tests passed\n", passed, total);
	return (passed == total) ? 0 : 1;
}
