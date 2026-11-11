# Project context for language-model assistants

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Current scope
The user asked for separate driver types and platform bindings, with common protocol logic/HAL reused where the device protocol is actually the same. Target set: Windows, Linux, Android, macOS, and FreeRTOS; document other OS targets such as Zephyr, QNX, VxWorks, FreeBSD, and embedded Linux. Device categories called out: USB, Bluetooth, Wi-Fi, PCI/PCIe, network, storage/block, robots/control systems, accelerators, and software-enumerated test devices.

## Current reference code
- `Shared/` contains the portable C protocol core, HAL ABI header, C tests, and C++ link test. PING/ECHO is a test/reference protocol only.
- `UMDF 2 Driver1/` is a Windows UMDF reference adapter for a root-enumerated software device.
- `Linux/` contains a root-only misc-device test module and userspace test client; WSL lacks matching kernel headers.
- `Android/` contains an AIDL service starter; the NDK/AOSP/Soong environment is not installed here.
- `MacOS/` contains only a protocol bridge. It is not a loadable DriverKit extension; Xcode/macOS is required.
- `FreeRTOS/` contains a static task/queue starter. A concrete BSP, FreeRTOS version and toolchain are required.
- `DriverTest/` contains a Python device-interface runner, class catalog, model profiles and QEMU scripts.

## Build and verification discipline
Run `make -C Shared test` in WSL for core C/C++ tests and `make -C Linux user-client` for the Linux user client. Build the Windows UMDF project with the installed WDK and test-sign only in a disposable VM. Mark native builds and hardware tests as unverified when unavailable. No native build of a platform starter is implied by its presence in the tree.

## Scope and safety limits
No concrete physical device IDs, wire protocols, firmware contracts, board/BSPs, hazard analysis, or safety standards have been provided. Do not invent these. Do not claim that generic QEMU devices reproduce arbitrary USB, Bluetooth, Wi-Fi, PCI, storage, robot, or accelerator hardware. Do not claim SIL/ASIL, certification, or production readiness.

## Licensing
Use GPL-3.0-only for original repository material, retain the requested copyright notice, and keep third-party/vendor provenance separate. Never overwrite another author's notice or assume a dependency is GPL-compatible without review. The user wants local Git only, no remote or publish action, and an initial commit; do not fabricate Git author email.
