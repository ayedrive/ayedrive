# Critical-systems validation plan and evidence framework

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Important boundary
This document is a planning checklist, not a safety case, approval, certification, or claim that this repository is suitable for a safety-critical product. The applicable process and standard must be selected by the responsible organization for the actual domain and jurisdiction. A successful build, unit test, VM test, or code review is not sufficient for safety approval.

## 1. Required project inputs
Before safety-related implementation begins, record: intended function and operating environment; foreseeable misuse; device identity and firmware; affected people/assets; hazard severity/exposure/controllability as applicable; safe state; failure detection and response times; platform/board; system boundary; external dependencies; cybersecurity threat model; and the designated safety authority/independent reviewer.

## 2. Standards selection
Have a qualified safety/compliance owner identify applicable editions and scope. Examples to evaluate include IEC 61508 for functional safety, ISO 26262 for road vehicles, IEC 62304/ISO 14971 for medical software/risk, IEC 61511 for process industry, EN 50128/50129 for rail, DO-178C/DO-254 for avionics software/hardware, and IEC 62443 for industrial cybersecurity. This list is illustrative, not a determination that any standard applies. Do not state SIL, ASIL, DAL, certification, or compliance without formal scope, required lifecycle evidence, and approval.

## 3. Hazard and risk lifecycle
1. Define intended use, system boundary, operational modes, and interfaces.
2. Perform hazard analysis using a suitable method (for example HAZOP, FMEA/FMECA, fault tree, STPA, or a justified combination).
3. Derive safety requirements with unique IDs, measurable thresholds, owner, verification method, and traceability.
4. Allocate requirements to hardware, firmware, driver, OS, application, operator, and maintenance controls.
5. Analyze common-cause failures, resource exhaustion, timing, power loss, reset, update, and degraded-mode behavior.
6. Review residual risk and safe-state behavior with independent competent reviewers; retain approval evidence.
7. Reassess after device/firmware/OS/toolchain changes and operational incidents.

## 4. Driver-specific failure analysis checklist
- Bad, truncated, oversized, reordered, replayed, or unsupported protocol messages.
- Integer overflow, alignment, endianness, untrusted identifiers, and malformed descriptors.
- Buffer lifetime, aliasing, use-after-free, double completion, cancellation races, timeout, queue exhaustion, and reentrancy.
- Device removal, surprise removal, reset, firmware restart/update, suspend/resume, low-power transition, and failed initialization.
- DMA/IOMMU mapping, cache coherency, address width, interrupt storm/loss, PCIe error/reset, and bus contention.
- Storage torn writes, flush/barrier semantics, media wear/error, power loss, and corrupted metadata.
- Network packet floods, link flap, malformed frames, offload mismatch, and firmware recovery.
- Robot actuator limits, stale sensor input, watchdog, communication loss, safe stop, and restart interlocks.
- Accelerator queue corruption, invalid DMA, firmware/runtime mismatch, timeout, thermal/power limits, reset and recovery.
- Unauthorized access, privilege escalation, debug paths, insecure update, key handling, and diagnostic data leakage.

## 5. Verification evidence
Every safety requirement shall trace to design elements and verification evidence. The evidence plan should include:

| Stage | Minimum evidence examples |
|---|---|
| Requirements/design | Hazard traceability, interface contracts, state machines, review records, failure analysis |
| Static | Compiler diagnostics, static analysis, secure coding review, dependency/provenance and license scan |
| Unit | Boundary, negative, state transition, overflow, error, timeout and injected-failure tests |
| Integration | HAL contract, native buffer mapping, synchronization, cancellation, power and removal tests |
| System | VM/simulator where meaningful, exact OS/firmware versions, PnP and upgrade/rollback scenarios |
| HIL | Controlled fixture, exact hardware/firmware IDs, sensor/actuator limits, fault injection, calibration, repeatability |
| Release | Reproducible build inputs, signing chain, SBOM/provenance, test records, known limitations, approval and rollback plan |

## 6. Independence and change control
- Separate author and reviewer for safety-critical requirements, design, and verification where the selected standard requires independence.
- Review every change to protocol, HAL ABI, hardware profile, firmware, OS version, compiler, signing chain, or test fixture for safety impact.
- Preserve immutable source revisions, tool versions, test logs, instrumentation/calibration records, review approvals, and deviations.
- Re-run regression evidence based on documented impact analysis; no undocumented waiver of failed tests.

## 7. Release gate
A release is blocked until the responsible authority has approved scope/standard, hazard analysis, traceability, independent review, target-platform tests, hardware evidence, security analysis, residual-risk acceptance, license/provenance, installation/recovery instructions, and operational monitoring plan. This repository does not currently satisfy or claim this gate.
