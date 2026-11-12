/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DEVICE_TYPES_H
#define DEVICE_TYPES_H

#if defined(__KERNEL__)
#include <linux/stddef.h>
#include <linux/types.h>
typedef u8 dpc_u8;
typedef u16 dpc_u16;
typedef u32 dpc_u32;
typedef u64 dpc_u64;
typedef size_t dpc_size_t;
#else
#include <stddef.h>
#include <stdint.h>
typedef uint8_t dpc_u8;
typedef uint16_t dpc_u16;
typedef uint32_t dpc_u32;
typedef uint64_t dpc_u64;
typedef size_t dpc_size_t;
#endif

#endif
