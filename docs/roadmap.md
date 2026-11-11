# Portfolio roadmap and current status

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Status meanings
- **Implemented/tested:** source exists and the stated build/test ran successfully.
- **Starter/unverified:** source scaffolding exists, but native SDK build or target test has not run.
- **Requirements-only:** profile/checklist exists; no device-specific implementation exists.
- **Documented:** extension path is described; no source binding exists.

## Portfolio

| Component | Status | Next dependency |
|---|---|---|
| Portable reference core and C unit tests | Implemented/tested in WSL GCC; 5/5 tests pass with strict warnings | Protocol from actual hardware owner |
| C++ core linkage test | Implemented/tested in WSL G++; links C-compiled core | Native Android AIDL build |
| Windows UMDF root software reference | Debug x64 WDK build and Inf2Cat signability test pass with no warnings/errors | Disposable Windows VM integration and real device profile |
| Linux userspace test client | Compiles in WSL with strict warnings | Linux module and device node |
| Linux misc-device module | Starter/unverified; matching WSL kernel headers unavailable | Matching Linux headers/config and isolated Linux VM |
| Android AIDL service | Starter/unverified; Android NDK/AOSP build not available here | AOSP/Soong/NDK branch, service policy, target device |
| macOS protocol bridge | Implemented as a bridge only, not a driver; native build unavailable here | macOS/Xcode and supported DriverKit device class |
| FreeRTOS queue/task binding | Starter/unverified; FreeRTOS SDK/board toolchain unavailable here | Named FreeRTOS version, board/BSP, compiler and lifecycle completion |
| USB/Bluetooth/Wi-Fi/PCI/storage/network/robot/accelerator profiles | Requirements-only | Exact IDs, protocol, firmware and per-platform device owner |
| Zephyr/QNX/VxWorks/FreeBSD/other OS ports | Documented | Exact OS/version/framework and target |
| QEMU Windows runner/media scripts | Shell syntax passes; prior dry-run passed | xorriso, Windows guest disk/media, guest install and live test |
| Python driver-test harness | 6/6 unit tests pass | Live Windows device-interface/VM test |
| Critical-systems readiness framework | Domain screening, profile schema, assessment/safety-case/review templates, and preliminary non-safety sample assessment drafted | Select an actual product/use case, obtain qualified standards decisions, complete analyses, collect target evidence, and secure independent approval |
| Local Git repository | Initial commit exists; repository remains remote-free | User chooses hosting location if desired |

## Release sequence
1. Provide concrete hardware dossier and select one family/OS target.
2. Validate protocol and HAL contracts against device specification.
3. Implement and review one native binding; add tests and build evidence.
4. Build exact native target and exercise in VM/simulator.
5. Validate on exact hardware/firmware with safe fixtures.
6. Complete security, safety, provenance, independent review, training and release gates.
7. Repeat per family/platform rather than claiming broad support from one sample.

The critical-systems documentation milestone is complete as a reusable framework only. No product deployment has an approved hazard analysis, security assessment, applicable-standards decision, safety case, target/HIL evidence, independent review, or certification. The current reference sample must remain non-safety development/training code.
