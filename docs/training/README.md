# Driver development learning and training program

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Audience and prerequisites
For C developers, firmware engineers, and test engineers learning to build driver portfolios across operating systems. Before starting, know C pointers/arrays/structs, compile and link a C program, use a command shell, read logs, and understand that driver code can crash or corrupt a machine. Perform labs only in disposable VMs/boards. Do not load an experimental driver on a daily-use or production system.

## Learning outcomes
A learner who completes all modules should be able to: explain kernel/user-mode boundaries; design bounded wire protocols; define a transport-specific HAL; keep protocol logic portable; implement one native binding; create fake/simulated hardware tests; diagnose PnP, power, cancellation, DMA and concurrency defects; maintain requirements-to-test traceability; and distinguish test evidence from safety certification.

## Learning path

### Module 0 — Safe lab setup and licensing
**Objectives:** distinguish host, guest, and target board; understand test-signing, snapshots, rollback, third-party SDK licenses, and source provenance.
**Lab:** create a disposable Windows or Linux VM; snapshot it; verify that no host physical disk is attached; inspect GPL and SPDX notices.
**Assessment:** describe rollback, signing, data-loss, and license-provenance risks before proceeding.

### Module 1 — Portable C and boundary-safe programming
**Objectives:** fixed-width types, pointer ownership, integer overflow, alignment, lifetimes, endian encoding, error returns, static analysis, and compiler warnings.
**Lab:** build `Shared/`; add malformed/truncated/oversized frames; compile with `-Wall -Wextra -Wpedantic -Werror` and sanitizers when available.
**Assessment:** show each buffer access is preceded by a validated range and explain why wire structs are not compiler-packed ABI.

### Module 2 — Protocol, state machine, and HAL design
**Objectives:** protocol versioning, capabilities, synchronous/asynchronous requests, timeout, cancellation, reset, retries, backpressure, and error mapping.
**Lab:** implement a new test-only command in a copy of the reference protocol; test old-version and unsupported-command behavior.
**Assessment:** produce a protocol state diagram, HAL operation table, buffer ownership rule, and failure-response table.

### Module 3 — Unit testing and fault injection
**Objectives:** test oracles, property/boundary tests, fuzzing, sanitizers, deterministic fake HALs, log correlation, and race conditions.
**Lab:** add unit tests for invalid lengths, insufficient output, cancellation, timeout, and device removal.
**Assessment:** provide a test report that includes compiler, flags, source revision, and pass/fail results.

### Module 4 — Windows driver models
**Objectives:** WDF/UMDF vs KMDF distinctions; INF and PnP matching; device interfaces; IOCTL access bits; I/O buffer methods; queues; power management; signing and Driver Verifier.
**Lab:** build the UMDF reference project, install only in a test VM, run the sample protocol test, inspect PnP status and WDF traces, then remove the device and driver package.
**Assessment:** explain how IOCTL code, buffer methods, access control, PnP lifetime, and completion information align.

### Module 5 — Linux kernel and userspace boundaries
**Objectives:** Kbuild, kernel/user ABI, copy_from_user/copy_to_user, ioctl numbering, misc/character/network/block/USB/PCI drivers, locking, DMA APIs, module signing and kernel logs.
**Lab:** build the user client in WSL; on a separate Linux VM with matching headers, build/load the root-only reference module and run the PING test.
**Assessment:** review user-pointer handling, ioctl compatibility, permissions, cleanup paths, and unload safety.

### Module 6 — Android driver and HAL integration
**Objectives:** Linux kernel driver versus Android HAL/service; AIDL versioning, system/vendor boundaries, SELinux, init/VINTF manifests, binder concurrency, power and permission policy.
**Lab:** in an AOSP/vendor build environment, freeze/version an AIDL test interface and route it to a target test device node.
**Assessment:** provide AIDL compatibility, SELinux, permission, and kernel/HAL error-mapping evidence.

### Module 7 — macOS DriverKit
**Objectives:** system extensions, supported DriverKit device families, user clients/external methods, entitlements, signing/notarization, extension lifecycle, and compatibility by OS release.
**Lab:** on supported macOS/Xcode, create a native DriverKit target for an explicitly supported device family and invoke the shared protocol bridge.
**Assessment:** build/sign/install/uninstall on a disposable Mac and document entitlement and device-matching constraints.

### Module 8 — FreeRTOS and embedded ports
**Objectives:** task/ISR separation, queues, static allocation, scheduler priority, interrupt priorities, DMA/cache coherency, stack budgeting, watchdogs, and board support packages.
**Lab:** port the protocol task to a named FreeRTOS release/board, first with loopback buffers and then with a supported peripheral.
**Assessment:** produce stack/heap/queue budgets, ISR rules, timeout/cancel semantics, and target timing evidence.

### Module 9 — Device-family laboratories
Select one family and complete its profile: USB descriptors/endpoints; Bluetooth HCI and radio/profile behavior; Wi-Fi scan/association/offloads; PCI config/BAR/DMA/interrupts; storage geometry/flush/power-loss; network packet/link/offloads; robot sensor/actuator safe state; or accelerator queues/memory/reset. Use an emulator only where it models the exact behavior under test.
**Assessment:** profile completeness review by firmware/platform/test owners before code starts.

### Module 10 — Virtualization and hardware-in-the-loop
**Objectives:** QEMU machine/device model vs passthrough, VM snapshots, guest signing, test media, transport limitations, disposable block media, and HIL fixture control.
**Lab:** compare one modeled virtual device with its physical target and list behavior that the emulator does not cover.
**Assessment:** label every result as host unit, simulated integration, VM, or physical HIL evidence.

### Module 11 — Critical-system validation and release engineering
**Objectives:** hazard analysis, FMEA/FMECA, traceability, independent review, static analysis, fault injection, reproducible builds, signing, SBOM/provenance, change control, and incident/rollback plans.
**Lab:** create a mock safety case and trace each hazard to requirements, design controls, tests, and residual-risk acceptance.
**Assessment:** a qualified independent reviewer must approve any real safety-related release; a training exercise is not certification.

## Capstone
Choose a real device with authorized specifications. Write its profile and requirements, implement one platform binding and shared protocol logic, add fake-HAL tests, run a simulator/VM test, run a controlled HIL test, and submit traceability, threat/safety review, build evidence, release notes, and known limitations. Repeat on a second OS only where the protocol is truly shared.

## Competency gates
1. **Foundation:** C safety and host tests pass.
2. **Protocol:** message and HAL contracts reviewed.
3. **Platform:** native build plus PnP/power/cancellation tests pass.
4. **Device:** exact hardware/firmware and HIL evidence captured.
5. **Release:** license/provenance, security, safety, and independent approval complete.

## Suggested references
Use the official WDK/WDF documentation, Linux kernel driver API and subsystem documentation for the target kernel, Android AIDL/Soong/SELinux documentation for the target release, Apple DriverKit documentation for the supported device family, and the selected FreeRTOS release/BSP documentation. Always use version-matched references; APIs and requirements change between releases.
