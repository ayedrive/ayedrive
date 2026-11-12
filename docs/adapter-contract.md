# Native adapter integration contract

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Separation
A native adapter owns OS objects, enumeration, user/kernel marshaling, synchronization, power/removal, bus mapping, DMA/interrupt plumbing, permissions, and native status conversion. The portable core owns only protocol/state behavior. The platform backend implements the versioned HAL contract and reports only capabilities that are concretely available. Do not copy OS structs, handles, pointers, or lock objects into the wire format or common core.

## Required source markers
Every completed native hardware adapter records these markers in its reviewed source/design and traceability report:
- `[OS-LIFECYCLE]` — start/stop, enumeration, reset, suspend/resume, surprise removal, teardown.
- `[OS-USER-IO]` — validation, authorization, length/capacity, buffer lifetime and native status mapping.
- `[OS-MMIO-MAP]` — resource ownership, bounds, width, endianness, ordering and side effects.
- `[OS-DMA-ARENA]` — address width, IOMMU policy, SG, cache sync, alignment, cleanup.
- `[OS-IRQ-WIRE]` — top-half constraints, deferred work, vector lifecycle, rate/error handling.

A userspace-only service or protocol bridge must explicitly state inapplicable markers and name the kernel/OS component that owns them. Lack of a marker means not implemented, not implicitly supported.

## HAL groups
The eventual hardware HAL is versioned/capability-gated and separates bus/register operations, DMA, split-context IRQ, monotonic time/timers, ISR-safe versus sleepable synchronization, and optional VFS projection. A platform may implement only a subset; absent callbacks are `NOT_SUPPORTED`, never success-no-op. The current `Shared/include/device_hal.h` is a protocol transfer/lifecycle contract, not yet this full six-group bus HAL. Do not silently replace its ABI; introduce a new reviewed interface/version with contract tests before migration.

## Status matrix
`DriverTest/models/adapter-capabilities.json` is the current inventory. It records that only the Windows UMDF reference build, Linux userspace test client, and portable host core have specific build evidence. Linux kernel, Android, macOS, FreeRTOS and all other native adapters remain unverified, bridge-only or requirements-only. Update that registry only with reproducible logs for the exact target/compiler/SDK and tests.
