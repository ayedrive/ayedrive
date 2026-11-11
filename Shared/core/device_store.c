/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "device_store.h"

#include <stddef.h>

uint32_t dpc_store_crc32(const uint8_t *data, uint32_t length)
{
	uint32_t crc = UINT32_C(0xFFFFFFFF);
	uint32_t index = 0U;
	uint32_t bit = 0U;
	uint32_t mask = 0U;

	if (data == NULL) {
		return (length == 0U) ? UINT32_C(0) : UINT32_C(0);
	}

	for (index = 0U; index < length; ++index) {
		crc ^= (uint32_t)data[index];
		for (bit = 0U; bit < 8U; ++bit) {
			mask = UINT32_C(0) - (crc & UINT32_C(1));
			crc = (crc >> 1U) ^ (UINT32_C(0xEDB88320) & mask);
		}
	}
	crc ^= UINT32_C(0xFFFFFFFF);
	return crc;
}

static int bank_is_valid(const dpc_store_bank *bank)
{
	int valid = 0;

	if (bank != NULL) {
		valid = (bank->commit_marker == DPC_STORE_COMMIT_MARKER) &&
				(bank->length <= DPC_STORE_CAPACITY) &&
				(bank->crc32 == dpc_store_crc32(bank->bytes, bank->length));
	}
	return valid;
}

dpc_store_status dpc_store_format(dpc_store *store, const uint8_t *initial, uint32_t length)
{
	uint32_t index = 0U;
	uint32_t bank_index = 0U;
	dpc_store_status result = DPC_STORE_INVALID;

	if ((store != NULL) && (length <= DPC_STORE_CAPACITY) && ((initial != NULL) || (length == 0U))) {
		for (bank_index = 0U; bank_index < 2U; ++bank_index) {
			for (index = 0U; index < DPC_STORE_CAPACITY; ++index) {
				store->banks[bank_index].bytes[index] = 0U;
			}
			for (index = 0U; index < length; ++index) {
				store->banks[bank_index].bytes[index] = initial[index];
			}
			store->banks[bank_index].length = length;
			store->banks[bank_index].generation = 0U;
			store->banks[bank_index].crc32 = dpc_store_crc32(store->banks[bank_index].bytes, length);
			store->banks[bank_index].commit_marker = DPC_STORE_COMMIT_MARKER;
		}
		store->active_bank = 0U;
		result = DPC_STORE_OK;
	}
	return result;
}

dpc_store_status dpc_store_select(dpc_store *store)
{
	int valid_zero = 0;
	int valid_one = 0;
	dpc_store_status result = DPC_STORE_NO_VALID_BANK;

	if (store != NULL) {
		valid_zero = bank_is_valid(&store->banks[0]);
		valid_one = bank_is_valid(&store->banks[1]);
		if (valid_zero && valid_one) {
			store->active_bank = (store->banks[1].generation > store->banks[0].generation) ? 1U : 0U;
			result = DPC_STORE_OK;
		} else if (valid_zero) {
			store->active_bank = 0U;
			result = DPC_STORE_OK;
		} else if (valid_one) {
			store->active_bank = 1U;
			result = DPC_STORE_OK;
		}
	}
	return result;
}

dpc_store_status dpc_store_read(const dpc_store *store, uint32_t offset, uint8_t *output, uint32_t length)
{
	uint32_t index = 0U;
	const dpc_store_bank *bank = NULL;
	dpc_store_status result = DPC_STORE_INVALID;

	if ((store != NULL) && ((output != NULL) || (length == 0U)) && (store->active_bank < 2U)) {
		bank = &store->banks[store->active_bank];
		if (bank_is_valid(bank)) {
			if ((offset <= bank->length) && (length <= (bank->length - offset))) {
				for (index = 0U; index < length; ++index) {
					output[index] = bank->bytes[offset + index];
				}
				result = DPC_STORE_OK;
			}
		} else {
			result = DPC_STORE_CORRUPT;
		}
	}
	return result;
}

dpc_store_status dpc_store_commit(dpc_store *store, uint32_t offset, const uint8_t *input, uint32_t length, dpc_store_cut_point cut_point)
{
	uint32_t index = 0U;
	uint32_t active = 0U;
	uint32_t inactive = 0U;
	uint32_t new_length = 0U;
	dpc_store_bank *target = NULL;
	const dpc_store_bank *source = NULL;
	dpc_store_status result = DPC_STORE_INVALID;

	if ((store != NULL) && ((input != NULL) || (length == 0U)) && (cut_point <= DPC_STORE_CUT_AFTER_MARKER) &&
		(store->active_bank < 2U)) {
		active = store->active_bank;
		source = &store->banks[active];
		if (!bank_is_valid(source)) {
			result = DPC_STORE_CORRUPT;
		} else if ((offset > DPC_STORE_CAPACITY) || (length > (DPC_STORE_CAPACITY - offset))) {
			result = DPC_STORE_INVALID;
		} else if (source->generation == UINT32_MAX) {
			result = DPC_STORE_INVALID;
		} else {
			inactive = UINT32_C(1) - active;
			target = &store->banks[inactive];
			target->commit_marker = 0U;
			if (cut_point == DPC_STORE_CUT_AFTER_INVALIDATE) {
				result = DPC_STORE_INTERRUPTED;
			} else {
				for (index = 0U; index < DPC_STORE_CAPACITY; ++index) {
					target->bytes[index] = source->bytes[index];
				}
				new_length = source->length;
				if ((offset + length) > new_length) {
					new_length = offset + length;
				}
				for (index = 0U; index < length; ++index) {
					target->bytes[offset + index] = input[index];
				}
				for (index = new_length; index < DPC_STORE_CAPACITY; ++index) {
					target->bytes[index] = 0U;
				}
				target->length = new_length;
				target->generation = source->generation + 1U;
				if (cut_point == DPC_STORE_CUT_AFTER_DATA) {
					result = DPC_STORE_INTERRUPTED;
				} else {
					target->crc32 = dpc_store_crc32(target->bytes, target->length);
					if (cut_point == DPC_STORE_CUT_AFTER_CRC) {
						result = DPC_STORE_INTERRUPTED;
					} else {
						target->commit_marker = DPC_STORE_COMMIT_MARKER;
						if (cut_point == DPC_STORE_CUT_AFTER_MARKER) {
							result = DPC_STORE_INTERRUPTED;
						} else if (bank_is_valid(target)) {
							store->active_bank = inactive;
							result = DPC_STORE_OK;
						} else {
							result = DPC_STORE_CORRUPT;
						}
					}
				}
			}
		}
	}
	return result;
}
