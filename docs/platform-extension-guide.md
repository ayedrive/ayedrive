# Adding other operating systems and platforms

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Porting rule
A new port supplies a native device binding; it does not copy OS APIs into `Shared/`. Before implementation, write a platform profile with OS/version, compiler/SDK, supported device families, bus APIs, memory/DMA model, interrupt rules, power/lifecycle contract, signing/permissions, test strategy, and license requirements. Compile and test on the native target before marking the port supported.

## Extension targets

| Target | Likely native integration | Must resolve first |
|---|---|---|
| Zephyr | Device model, devicetree, bus/driver APIs, Kconfig and west build | Zephyr release, board, devicetree binding, driver class, DMA/cache/ISR rules |
| QNX | Resource manager or QNX driver framework, io-pkt/network, storage or bus APIs | QNX release, BSP, licensing/SDK, privilege model, resource-manager lifecycle |
| VxWorks | VxBus/device driver framework and vendor BSP | VxWorks release, BSP, toolchain, certification profile, interrupt/DMA and runtime policies |
| FreeBSD | Newbus, USB, net, block, HID or other kernel subsystem | FreeBSD release, kernel config, subsystem contracts, kmod build and device IDs |
| Embedded Linux | Linux kernel/subsystem plus board/device-tree and userspace integration | Kernel commit/config, vendor BSP, device tree, firmware, boot chain and reproducible toolchain |
| Hypervisors | Hypervisor-specific virtual device model or paravirtual transport | Guest/host ABI, migration/reset semantics, isolation, DMA/IOMMU and security review |
| Other RTOS | Native board/device framework or a narrowly scoped BSP adapter | Named RTOS/version, scheduler/interrupt rules, board, compiler, middleware and vendor licenses |

## Port review gate
- Show protocol compatibility and golden vectors against `Shared/`.
- Document any behavior that cannot be represented by the common HAL; extend the HAL only with a versioned, capability-gated change.
- Validate cleanup, cancellation, surprise removal, reset, power transition, resource exhaustion, and malformed input.
- Use that OS's static analysis, driver verifier, kernel test framework, sanitizers or equivalent.
- Add a simulator only if it models the exact bus/device behavior; mark approximations and passthrough explicitly.
- Update MRD/SRS traceability, training, test plan, platform matrix, license/provenance, and release evidence.

The repository currently documents these targets only; it does not contain runnable Zephyr, QNX, VxWorks, FreeBSD, or hypervisor drivers.
