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
| Portable reference core and C unit tests | Implemented/tested in WSL GCC; protocol 5/5 and HAL simulator 4/4 pass with strict warnings | Protocol from actual hardware owner |
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
| Device dossier automation | JSON profile/dossier schemas, intake template, validation/report/package/evidence-index CLI, hash inventory verification, and deterministic intake artifacts | Platform-specific read-only collectors, identity-backed signing, reviewed device dossiers and source-system integration |
| HAL protocol simulator | Deterministic synchronous PING/ECHO backend behind existing HAL ABI; virtual time, timeout checks and one-shot transport fault injection; 4 strict C11 tests pass | Device-specific model, asynchronous/concurrency model, broader fault campaigns and native adapter integration |
| C11 transactional-store virtual reference | In-memory dual-bank/CRC model and 5 fault/bounds tests pass strict C11 warnings | Backend persistence contract, additional fault campaigns, sanitizers/formal verification, QEMU device model and HIL |
| Factory automation and learning site | GitHub Actions workflow covers schemas, Python tests, C/C++ tests, unsigned evidence inventory, open-tool reporting, static notebook/doc export and Pages deployment configuration | Execute on hosted CI; enable Pages in repository settings; native licensed runners and assurance tools remain separate gates |
| Ephemeral QEMU guest workflow | User-supplied media/checksum workflow with disk preflight, network/host-device isolation and cleanup; shell syntax validated | Install QEMU, obtain licensed OS media and checksum, confirm scratch capacity, then run disposable guest validation |
| Local Git repository | Initial commit exists; repository remains remote-free | User chooses hosting location if desired |

## Release sequence
1. Provide concrete hardware dossier and select one family/OS target.
2. Validate protocol and HAL contracts against device specification.
3. Implement and review one native binding; add tests and build evidence.
4. Build exact native target and exercise in VM/simulator.
5. Validate on exact hardware/firmware with safe fixtures.
6. Complete security, safety, provenance, independent review, training and release gates.
7. Repeat per family/platform rather than claiming broad support from one sample.

The framework's current verification is limited to WSL strict portable C/C++ tests, Python automation tests, schema validation, workflow parsing, and shell syntax checks. No QEMU guest or licensed OS media was run, no native target SDK builds or physical HIL tests were performed, and generated SHA-256 inventories are not signed attestations. No product deployment has an approved hazard analysis, security assessment, applicable-standards decision, safety case, target/HIL evidence, independent review, or certification. The current reference sample must remain non-safety development/training code.
