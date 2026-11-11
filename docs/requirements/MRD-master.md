# Master Requirements Document

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

**Status:** Draft engineering baseline. It describes the intended portfolio, not a claim that every target is implemented.

## 1. Scope and configuration items
- **CORE:** portable C protocol and state-machine implementation.
- **HAL:** versioned cross-platform operation/lifecycle contract.
- **WIN:** Windows UMDF/KMDF and family-specific WDF bindings.
- **LIN:** Linux kernel and userspace bindings.
- **AND:** Android kernel/vendor and HAL/service integration.
- **MAC:** macOS DriverKit/system-extension integration.
- **FRT:** FreeRTOS task/queue and BSP binding.
- **MODEL:** explicit device-family profiles and simulators.
- **TEST:** unit, integration, VM, fault-injection, and hardware-in-loop test infrastructure.
- **DOC:** requirements, training, safety, platform status, and provenance.

## 2. Requirements and verification

| ID | Requirement | Priority | Verification / acceptance | Current status |
|---|---|---:|---|---|
| MR-001 | Keep protocol/state-machine logic in a portable C layer with no OS calls. | MUST | Static review and host build | Reference core exists |
| MR-002 | Define versioned, bounded message parsing with explicit byte order and no compiler-packed wire ABI. | MUST | Unit tests for valid, malformed, oversized, and unsupported messages | Reference PING/ECHO implemented |
| MR-003 | Define a versioned HAL for transfer, cancellation, reset, lifecycle, and monotonic time. | MUST | API review and contract tests per binding | Header exists; native coverage incomplete |
| MR-004 | Keep Windows, Linux, Android, macOS, and FreeRTOS bindings independent. | MUST | Build/source review per target | Starter code exists; native status varies |
| MR-005 | Keep each device family/profile independently described. | MUST | Catalog/profile review | Requested categories have templates |
| MR-006 | A profile must name exact device identity, protocol, firmware, topology, and behavior before physical support is claimed. | MUST | Profile completeness gate | No physical device specs supplied |
| MR-007 | All I/O boundaries validate lengths, capacities, access, status mapping, and buffer lifetime. | MUST | Static analysis, unit and fault injection | Core covers reference message; ports need review |
| MR-008 | Async submissions have one completion, defined cancellation, timeout, and buffer ownership semantics. | MUST | Contract tests, race/cancel tests | HAL contract documents intent; ports incomplete |
| MR-009 | Storage writes require isolated disposable media and explicit authorization. | MUST | Test harness policy and review | Documented; per-platform execution remains |
| MR-010 | Build and test claims identify exact OS, toolchain, hardware, and result. | MUST | CI/report review | Matrix records known limitations |
| MR-011 | Original project materials carry GPL-3.0-only and requested copyright. | MUST | Automated source audit and manual provenance review | Repository text audit performed; full provenance review pending |
| MR-012 | Third-party SDK/template material retains separate attribution and license; incompatible/unknown material is not relicensed by assertion. | MUST | Dependency/provenance review | Notice and process documented |
| MR-013 | Requirements have unique IDs and test traceability. | MUST | Traceability matrix review | This document establishes IDs |
| MR-014 | Critical-system validation includes hazard analysis, independent reviews, fault injection, target tests, and evidence retention. | MUST when applicable | Product-specific safety case, approval evidence and independent assessment | Framework/templates exist; no product assessment or certification |
| MR-015 | Local Git remains remote-free until the user chooses a hosting location. | MUST | `git remote -v` empty | Local repository and initial commit exist; no remote |
| MR-016 | Training materials include staged outcomes, labs, assessments, and prerequisites. | MUST | Curriculum review | Curriculum drafted |
| MR-017 | Every safety-relevant deployment shall have a controlled profile identifying intended use, domain(s), jurisdiction(s), exact product configuration, applicable standards/editions, safety role, hazards, and approving authority. | MUST when applicable | Profile schema validation and authority review | Schema/template exist; no deployment profile approved |
| MR-018 | Every deployment shall default to monitor-only; a product control function shall remain unavailable until product-specific authorization, safe-state, independent-protection, verification, and approval evidence is present. | MUST when applicable | Design review and target tests; unauthorized/invalid configuration fails closed | Policy specified; no runtime safety/control mechanism implemented |
| MR-019 | Reuse of shared core/HAL code shall not transfer safety classification, assurance level, safety case, or certification between products/platforms. | MUST | Architecture and release review | Policy documented; product reviews pending |

## 3. Interfaces
### Portable protocol
- Fixed version and operation fields; explicitly encoded little-endian integers.
- Every message includes bounded payload length and request identifier.
- Rejection behavior is deterministic for malformed, unsupported, or capacity-limited requests.

### HAL
- Version/size negotiation.
- Operations are capability-gated; unsupported operations return an explicit status.
- Submit/cancel/reset/lifecycle and completion ownership are defined independently from native handles.

### Platform binding
- Exposes the platform's native device model and permissions.
- Maps native request/cancel/power/device-removal semantics to core/HAL semantics.
- Owns native DMA, interrupt, descriptor, MMIO, queue, and firmware lifetimes.

## 4. Device-family requirements

Every family entry requires an independent hardware profile and binding selection. USB needs IDs/descriptors/endpoints; Bluetooth needs bus/HCI/radio/profile behavior; Wi-Fi needs NDIS/mac80211/vendor firmware and radio scenarios; PCI needs config space/BAR/interrupt/DMA/reset details; storage needs geometry/flush/error/power-loss semantics; network needs packet/offload/link behavior; robots need sensor/actuator safety and timing; accelerators need queue/memory/runtime/reset behavior.

The family catalog is a requirements checklist. It is not evidence that code for each family exists or that generic virtual hardware matches the physical unit.

## 5. Verification lifecycle
1. Review profile and protocol specification.
2. Test the portable core with fake HALs and sanitizers.
3. Build the native binding with the exact target SDK.
4. Exercise OS lifecycle and device-model integration in an isolated VM/simulator.
5. Run negative, boundary, concurrency, cancellation, reset, suspend/resume, and fault-injection tests.
6. Run on exact hardware/firmware revision and preserve logs, traces, and test versions.
7. Complete security, safety, licensing, and independent review gates before release.

## 6. Traceability table

| Requirement range | Primary evidence |
|---|---|
| MR-001–MR-003 | `Shared/`, host tests, HAL/API reviews |
| MR-004 | `docs/platform-matrix.md`, per-target build reports |
| MR-005–MR-006 | `DriverTest/models/`, supplied device specification and profile review |
| MR-007–MR-009 | Unit tests, platform I/O tests, security/fault test records |
| MR-010–MR-012 | Build logs, license/provenance inventory, dependency review |
| MR-013–MR-016 | Requirements/test matrix, safety plan, curriculum, Git metadata |
| MR-017–MR-019 | `docs/safety/`, controlled deployment profile, product-specific assessment and approval records |

## 7. Change control
Changes to wire format, HAL ABI, device identity, safety behavior, or platform support require a versioned requirements update, compatibility analysis, tests, review approval, and updated training/operational documentation. Never silently reinterpret existing wire fields.
