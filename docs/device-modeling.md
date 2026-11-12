# Device model and family profile procedure

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Purpose
A profile records facts needed to create one device-family binding and simulator. It is not a generic implementation and must not imply that different vendors or revisions share behavior. Templates are in `DriverTest/models/profiles/`. `DriverTest/models/device-spec.schema.json` provides a machine-readable requirements profile using six functional classes and four behavioral axes; `device-taxonomy.json` is the category catalog and `device-spec.example.json` is a software-only example. Neither file asserts an emulator or driver exists.

## Required device dossier
Before changing a requirements-only profile to supported, capture:
1. Manufacturer, product, exact model, hardware/revision IDs, subsystem IDs, serial policy, and firmware revision.
2. Physical bus topology and native OS enumeration/matching mechanism.
3. Protocol specification, byte order, framing, command/state tables, descriptors/registers/queues, payload limits, checksums, and version negotiation.
4. Transfer types, endpoint/queue topology, timeouts, retry/reorder semantics, cancellation, reset, suspend/resume, and removal behavior.
5. DMA/IOMMU/address-width/cache rules, interrupts, memory ownership, and alignment constraints.
6. Power/thermal behavior, watchdogs, error and safe-state behavior, firmware update/recovery, and security controls.
7. Supported OS versions, framework, compiler, SDK, signing/entitlement requirements, vendor dependencies, and license provenance.
8. Normal, boundary, malformed, fault, timing, recovery, upgrade, and rollback scenarios with expected observable results.
9. Virtualization/model fidelity table: which behavior is emulated, approximate, passed through, unsupported, or HIL-only.
10. Safety classification/standard owner if applicable; do not infer it from device category.

## Family-specific additions
- **USB:** VID/PID, device/configuration/interface/endpoint descriptors, alternate settings, control/bulk/interrupt/isochronous semantics, bandwidth and power.
- **Bluetooth:** USB/PCI/UART attachment, HCI version, commands/events, firmware, radio, profiles/services, pairing and coexistence.
- **Wi-Fi:** bus, NDIS/WDI or mac80211/vendor interface, firmware, bands/channels/regulatory constraints, association/security, scan, power-save, offloads.
- **PCI/PCIe:** config-space fields, BAR sizes/types, interrupt modes, DMA rings, IOMMU policy, reset hierarchy, AER/error behavior, power.
- **Storage/block:** logical/physical block size, capacity, queue limits, ordering/flush/FUA, caching, media error and power-loss behavior.
- **Network:** frame/link model, queueing, MTU, offloads, multicast/VLAN, counters, link state and recovery.
- **Robot/control:** sensor timing/units/calibration, actuator range/rate limits, interlocks, watchdog, stale data and safe stop.
- **Accelerator:** firmware/runtime, queue submission ABI, memory pin/map, DMA, completion, watchdog/reset, thermal and error reporting.

## Implementation gates
1. Profile reviewed by device/firmware owner.
2. Protocol specification and golden test vectors approved.
3. Shared behavior extracted only where byte-level semantics are identical.
4. Per-OS binding implements native lifecycle and bus semantics.
5. Simulator capability/gap matrix reviewed; unsafe operations disabled by default.
6. Host, native platform, VM, and hardware tests pass at their stated evidence levels.
7. Profile status changes to supported only after peer review of reproducible evidence.

## Specification profile fields

The JSON Schema requires device identity/firmware/protocol claims, one functional class, one or more transport types, transfer semantics, cache-coherency behavior, concurrency model, lifecycle model, target OS execution models, and a safety-profile reference. Use exact values only when supported by a device dossier. For software-only models, say so explicitly and use a `non_safety` profile; do not invent hardware IDs, firmware, or protocol compatibility.

The taxonomy is descriptive, not an implementation generator. QEMU's generic device models do not emulate arbitrary Bluetooth, Wi-Fi, PCI function, robot, accelerator, storage, or fieldbus behavior. A sim-to-real claim requires identical defined core semantics plus backend-specific conformance tests and a fidelity record.
