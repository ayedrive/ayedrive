# Target execution matrix and maturity rules

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

This matrix enumerates desired integration surfaces, not current support. A model becomes **verified** only with reproducible builds and tests for a named OS/SDK/device profile. A QEMU guest test is not a physical-device or safety qualification.

## Maturity levels

| Level | Meaning | Minimum evidence |
|---|---|---|
| L0 Documented | Requirements/target names only | Maintained contract and prerequisites |
| L1 Schema/profile | Dossier can be validated; no implementation claim | Schema validation and review |
| L2 Portable model | Device core plus deterministic simulator contract | C11 build, unit tests, seeded faults and reproducible traces |
| L3 Adapter build | Native adapter compiles against exact SDK | CI build log and API/lifecycle checks |
| L4 VM integration | Driver loads and exercises device model in guest | Exact image/build, QEMU version/args, model, install/test/cleanup report |
| L5 HIL | Exact hardware/firmware exercised with controlled fixture | Calibration, test logs, physical identity and independent review |
| L6 Safety release | Product-specific assurance process approved | Applicable authority's evidence, assessment and signed decision; never inferred from this repository |

## Operating systems and execution models

| Target | Desired adapter/model scope | Current baseline level | Gating dependencies |
|---|---|---:|---|
| Windows KMDF, UMDF v2, WDM | PnP, user I/O, MMIO, DMA, IRQ, power; legacy WDM only for justified cases | UMDF root reference has build only | WDK versions, disposable guest, hardware/device model; KMDF/WDM not implemented here |
| Windows NDIS, Storport, AVStream | Native miniport/class contracts | L0 | Relevant WDK samples, class tests, exact device profile |
| Linux platform/pci/usb/i2c/spi | Native subsystem drivers and lifecycle | L1 | Kernel source/config/headers, QEMU/KVM guest, device model |
| Linux UIO/VFIO/libusb/spidev | Explicit userspace access/ownership models | L0 | Kernel configuration, IOMMU/isolation and device; no host passthrough by default |
| Linux eBPF/XDP | Verifier-bounded data plane | L0 | Kernel feature set, verifier tests and traffic model |
| Android kernel + AIDL/HIDL HAL | Linux kernel device plus Android versioned service | L1 | AOSP branch, Soong, generated interfaces, SELinux, target image |
| macOS DriverKit (PCI/USB/HID/audio/network) | Supported class-specific system extension | L1 bridge only | macOS/Xcode/entitlements and supported hardware; Windows host cannot native-build |
| Legacy macOS IOKit KEXT | Migration/maintenance only where strictly required | L0 | Legacy OS and entitlement/security policy; not a new-driver default |
| QNX Neutrino resmgr/iofunc, pulse IRQ, typed DMA | POSIX resource manager and BSP contract | L0 | QNX SDK/license, BSP, target or licensed guest |
| FreeRTOS/SafeRTOS | Static allocation/notification/ISR profile | L1 | Named release, board/BSP/toolchain; SafeRTOS requires vendor licensing |
| Zephyr | Devicetree/Kconfig/device API | L0 | Zephyr SDK/west, board, native or QEMU-supported target |
| VxWorks, ThreadX, NuttX | BSP-specific RTOS adapters | L0 | Vendor SDK/license, board and certification context |
| QEMU QOM device models | Deterministic reference device emulation | L0 | QEMU development toolchain; QEMU not installed on current Windows/WSL environment |
| Renode | Peripheral/platform simulation | L0 | Renode installation and peripheral model validation |
| virtio/vhost-user | Paravirtual frontend/backend | L0 | Linux/QEMU and protocol conformance tests |
| Xen Dom0/DomU | Split driver, grant tables, event channels | L0 | Xen build/guest tooling and security review |
| Hyper-V VMBus | Native synthetic device frontend/backend | L0 | Windows SDK/Hyper-V capable host and guest |
| VMware VMCI/PVSCSI | Guest paravirtual drivers | L0 | Licensed VMware environment and compatibility tests |
| ACRN/Jailhouse/QNX Hypervisor ivshmem | Partitioned/real-time hypervisor bindings | L0 | Supported hardware, BSP, hypervisor SDK and isolation evidence |

## Six functional classes and behavioral dimensions

See `DriverTest/models/device-taxonomy.json` and `device-spec.schema.json`. Each dossier must classify the device as character/control, block/storage, network/fieldbus, continuous stream, actuator/motion/timing, or bus-controller/bridge, and record transfer mechanism, cache coherence, concurrency/latency, and lifecycle/power behavior. Cross-cutting concerns include protocol identity, authorization, DMA/IOMMU, interrupt semantics, deadlines, reset and recovery.

## Reference virtual models and truth policy

Initial target is a software-only transactional NVRAM/telemetry model and the existing PING/ECHO protocol reference. It is not a physical device. Add virtual models by named profile: model registers/queues, side effects, timing, reset, errors and fault injection; state unsupported or approximate behavior explicitly. QEMU's generic NIC/USB/PCI devices do not model arbitrary devices. Never label an unmodeled function as emulated.
