# Cross-platform test plan

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Test layers
1. **Host unit:** portable parser/state machine with deterministic fake inputs/HAL; no hardware.
2. **ABI/link:** C core compiled as C and called from C++.
3. **Binding unit:** native request-to-core mapping, lengths, error mapping, and completion behavior.
4. **VM/simulator:** exact modeled device and guest OS; record emulator coverage gaps.
5. **Hardware-in-the-loop:** exact hardware/firmware with controlled fixtures and safety limits.

## Shared-core cases
- Supported version and operation; empty and maximum-size payload.
- Unsupported operation/version and malformed/truncated frames.
- Length mismatch, integer boundary, zero/maximum/over-maximum capacity.
- Aliasing and input/output overlap where the API permits it.
- Response size negotiation; no output writes on rejected input.
- C and C++ linkage; compiler warnings and sanitizers.

## Platform matrix

| Target | Unit/build | Integration | Current status |
|---|---|---|---|
| Windows UMDF | WDK Debug x64 build and core test | VM install, device enumeration/interface, PING/ECHO IOCTL, malformed/unknown requests, uninstall | Existing UMDF/reference core build has passed; live guest tests pending |
| Linux kernel | Kbuild against exact headers/config, sparse/static checks | VM module load/unload, permissions, ioctl positive/negative, compat ABI | Not built: matching WSL headers unavailable |
| Linux userspace | GCC strict build | Run against module/device node | Client compiles; module absent in WSL |
| Android | Soong/AIDL build on target AOSP branch | Binder service, permission/SELinux, actual vendor node, lifecycle | Not built: NDK/AOSP/Soong unavailable |
| macOS | Xcode DriverKit target build/sign | Extension install/load/unload and supported device method | Bridge only; no native extension or Xcode environment |
| FreeRTOS | Named board/compiler/FreeRTOS configuration | Task/queue timing, static memory, ISR/DMA, peripheral tests | Not built: SDK/BSP unavailable |

## Model-specific tests
- **USB:** descriptors/configuration, endpoint types, short/long transfers, stalls, reset, suspend/resume, unplug.
- **Bluetooth:** HCI command/event sequencing, pairing/security, radio loss, profile behavior, coexistence.
- **Wi-Fi:** scan/association/authentication, link loss, regulatory settings, power-save, offload and recovery.
- **PCI/PCIe:** config match, BAR bounds, interrupt/queue, DMA/IOMMU, reset, power transition, error recovery.
- **Storage/block:** geometry, alignment, read-only smoke tests, flush/order, bad sectors, power interruption using expendable images only.
- **Network:** packet boundaries, offloads, multicast, link changes, resource pressure, malformed input.
- **Robot/control:** bounded commands, stale sensors, interlocks, watchdog, communication loss, safe-state and restart conditions.
- **Accelerator:** queue/ring bounds, memory mapping, runtime compatibility, timeout, reset and device loss.

## Test safety and reporting
Never attach host physical disks for write tests. Run signed experimental drivers only in disposable VMs or isolated lab devices. Record source commit, target OS/build, compiler/SDK, device IDs/revision/firmware, simulator/version, test command, output, reviewer, and limitations. A simulated pass must not be reported as a physical HIL pass.
