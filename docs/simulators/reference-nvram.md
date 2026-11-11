# Transactional NVRAM reference model

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Scope
`Shared/core/device_store.c` is a portable C11, fixed-capacity in-memory reference model. It has dual banks, a CRC32 integrity check, generation selection, bounds checks, and deterministic interruption injection at four commit points. It is not a physical NVRAM driver, flash translation layer, QEMU QOM device, persistent storage implementation, HIL result, or safety-qualified transaction mechanism.

## Commit sequence modeled
1. Select the active bank only when commit marker, length, and CRC validate.
2. Invalidate the inactive bank.
3. Copy the old image, apply the bounded update, set the new length/generation, and calculate the CRC.
4. Write the commit marker.
5. Update the in-memory active-bank hint. Recovery reselects the highest-generation valid bank, so interruption after marker models a completed transaction even if the hint update was lost.

The interruption tests model the commit marker write as atomic. They do **not** prove atomicity of a real flash word, torn-write behavior, flash erase/program constraints, write endurance, cache flush/barrier durability, brownout, or hardware reset. A physical backend must establish its own atomicity and durability contract, adapt the record format to erase/program geometry, and validate on exact hardware with fault injection. CRC detects accidental corruption; it is not authentication.

## Verification
`make -C Shared clean test` runs the protocol tests, HAL simulator tests, and store tests. The simulator suite checks PING response through the HAL vtable, deterministic virtual time, one-shot transport failure, timeout behavior, unsupported operations, lifecycle, and clock overflow. Its completion callback runs inline exactly once for each valid submission; cancellation is not supported. The store suite checks the standard CRC32 vector, normal commit/read, interruption before/after commit point, corrupt-new-bank fallback, and capacity boundaries. Run under sanitizers and a formal checker when installed; absence is an open verification gate.
