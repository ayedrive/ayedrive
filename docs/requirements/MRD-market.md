# Market Requirements Document (MRD)

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

**Status:** Draft baseline from user-provided needs; no market interviews or competitive research have been performed. This document does not assert market size, customer demand, or commercial viability.

## 1. Product statement
Create a maintainable portfolio of independent device-driver projects with a shared portable protocol core where reuse is valid, explicit HAL/platform bindings, model-based test infrastructure, and a complete learning and validation path.

## 2. Intended users
- Device-driver developers working across Windows, Linux, Android, macOS, and FreeRTOS.
- Hardware/firmware engineers supplying physical device protocols and board support.
- Test and validation engineers building VM, simulator, and hardware-in-the-loop tests.
- Learners who need staged exercises from portable C to native driver frameworks.
- Safety/compliance reviewers who need traceable requirements and evidence plans.

## 3. User problems
- Platform APIs differ, making it difficult to share protocol logic without mixing OS code into it.
- A test harness that targets only one device class cannot validate distinct USB, Bluetooth, Wi-Fi, PCI, block, robot, and accelerator families.
- Generic virtual hardware is often mistaken for physical-device fidelity.
- Driver learning material frequently omits lifetime, cancellation, DMA, power, signing, security, and failure testing.
- Safety claims require evidence and domain-specific standards, not just a successful build.

## 4. Portfolio scope
### Included
- Portable protocol/reference core and documented HAL contract.
- Separate platform bindings and project profiles, initially for Windows, Linux, Android, macOS, and FreeRTOS.
- Device-family requirement profiles for USB, Bluetooth, Wi-Fi, PCI/PCIe, network, storage/block, robot/control, accelerator, root software devices, and custom types.
- Host unit tests, platform-specific integration tests, VM/simulator workflows, and hardware-in-the-loop extension points.
- GPLv3 licensing, provenance practices, engineering requirements, learning/training curriculum, and critical-system validation guidance.

### Not implied
- One binary or one driver that supports every OS, bus, or device.
- Working emulation of arbitrary physical devices without device-specific models.
- Certified safety, security, reliability, performance, or regulatory compliance.
- A production-ready implementation for any unspecified physical device.
- Automatic support for every RTOS, kernel version, board, or vendor SDK.

## 5. Business/product requirements
- **MR-001 Separation:** Every supported platform/device family is an independently identifiable binding/profile.
- **MR-002 Reuse:** Protocol logic that is byte- and behavior-compatible across targets is implemented once in the portable core.
- **MR-003 Extensibility:** Adding an OS or device family does not require adding platform-specific APIs to the shared core.
- **MR-004 Testability:** Each profile states its identity, expected behavior, test adapter, virtualization fidelity, and unsupported cases.
- **MR-005 Evidence:** Every support claim links to build/test evidence and identifies OS, SDK, hardware, and version.
- **MR-006 Safety transparency:** The repository distinguishes development scaffolding from safety approval or certification.
- **MR-007 Learning:** Training materials include prerequisites, labs, expected outcomes, assessments, and safety precautions.
- **MR-008 License:** Original materials carry GPL-3.0-only identifiers and the requested copyright notice; third-party components keep their own provenance.
- **MR-009 Local control:** Git is initialized locally only; no remote is selected or publishing performed without user action.

## 6. Success measures
- Shared-core host tests pass with strict warnings and include malformed input, boundary, and C/C++ ABI tests.
- Each claimed platform binding has a native build report and a defined integration test; unsupported toolchains are clearly marked.
- Every requested device category has a profile/checklist; a profile is not counted as an implementation.
- A new device can be added by supplying its actual protocol and filling the profile, without modifying unrelated platform bindings.
- Documentation provides a reproducible beginner-to-advanced learning sequence and a traceable critical validation process.
- No unverified safety or hardware-emulation claims remain.

## 7. Constraints and dependencies
- Requires native OS SDKs, matching kernel headers, board support packages, device firmware, hardware IDs, protocol specifications, and test hardware for complete bindings.
- macOS DriverKit requires a supported macOS/Xcode environment and entitlements.
- Android HAL work depends on a chosen Android release, AIDL stability model, Soong, vendor integration, and SELinux policy.
- FreeRTOS work depends on a named RTOS release, board, compiler, vendor HAL, memory/DMA/cache rules, and interrupt priorities.
- QEMU is useful only for specific modeled devices and guest operating systems; it is not universal device emulation.

## 8. Risks
- Over-abstraction can hide native semantics or create unsafe buffer/lifetime assumptions.
- A reference protocol may be mistaken for a real device contract unless clearly labeled.
- Missing vendor specifications prevent meaningful device fidelity.
- Critical systems may require lifecycle evidence and independent approvals outside repository scope.

## 9. Acceptance
This MRD is accepted as a planning baseline when each requirement has an owner, implementation status, verification method, and documented dependency in `MRD-master.md` and the traceability matrix. Market fit remains unvalidated until customer research is completed.
