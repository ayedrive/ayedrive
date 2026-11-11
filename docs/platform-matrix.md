# Platform and driver-family support matrix

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

Status describes this repository, not the general capability of the operating system. `Planned` or `scaffold` is not a tested or deployable driver.

| Platform | First binding target | Driver families represented | Local build/test status | Requirements before production use |
|---|---|---|---|---|
| Windows | Existing UMDF sample plus a distinct shared-core binding | Root software device first; separate future UMDF/KMDF, USB, Bluetooth, Wi-Fi/NDIS, PCI, storage/Storport, HID, robot and accelerator models | WDK build available on host; the existing sample builds. New binding status must be recorded per build. | WDK/version, device IDs, hardware protocol, signing/deployment, PnP/power/security review, VM and physical target tests |
| Linux | Portable core host tests, then a distinct kernel/user-space binding | USB, Bluetooth, Wi-Fi/NDIS-equivalent, PCI, network, block/storage, HID, robot and accelerator | WSL GCC available; matching WSL kernel headers are absent, so a loadable WSL kernel module cannot be validated here. | Matching kernel headers/config, Kbuild, target kernel, bus/device model, permissions and hardware testing |
| Android | Shared core with a version-specific Android HAL/service binding; kernel work remains Linux | USB, Bluetooth, Wi-Fi, PCI and platform devices as permitted by the SoC/vendor stack | Android NDK was not found. Not built here. | Exact Android release, AIDL/HIDL generation, NDK/Soong/SDK, vendor image, SELinux policy, device tree and hardware |
| macOS | DriverKit/system-extension starter, conditional on supported device class | Supported USB/HID/network/device classes; bus and class support varies | Xcode/macOS SDK is unavailable on this Windows/WSL host. Not built here. | macOS/Xcode, entitlements, signing/notarization, supported DriverKit APIs and Apple hardware |
| FreeRTOS | Portable-core port boundary and board-specific adapter | USB, Bluetooth, Wi-Fi, PCI, block, robot and accelerator only where the board/vendor stack supports them | FreeRTOS SDK/board toolchain was not found. Not built here. | Named FreeRTOS version, board/BSP, compiler, vendor HAL, interrupt/DMA/cache rules and target tests |
| Zephyr, QNX, VxWorks, FreeBSD, other embedded/desktop OSes | Documented extension targets | Add per-OS family ports as applicable | Not implemented or built | Exact OS/version, framework, SDK, target board/device and licensing review |

## Requested device categories

The model catalog distinguishes USB, Bluetooth, Wi-Fi, PCI/PCIe, network, storage/block, robot/control systems, accelerators, root-enumerated software devices, and custom devices. A category entry is only a checklist: matching a physical device requires its exact IDs, protocol, firmware, bus behavior, timing, power, reset, and error semantics.

The shared C core should include only behavior that is part of the same device protocol on every target. Transport mechanics and operating-system APIs stay in bindings. If a device protocol differs by firmware or vendor, it must be versioned/profiled rather than forced into a single generic operation set.
