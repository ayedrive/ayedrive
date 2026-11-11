# System and Software Requirements Specification

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## 1. Purpose
Specify the software boundary for shared protocol code and separate native bindings. Requirements use `shall` for mandatory behavior. The reference PING/ECHO protocol is a test fixture only.

## 2. Functional requirements
- **SRS-CORE-001:** The core shall accept a byte buffer and explicit input/output lengths; it shall not access beyond either bound.
- **SRS-CORE-002:** The core shall reject unsupported protocol versions and malformed length fields without producing a success response.
- **SRS-CORE-003:** The core shall encode/decode wire fields explicitly and shall not expose compiler struct layout as the wire ABI.
- **SRS-CORE-004:** The core shall use bounded payload sizes and shall report required output capacity without writing out of bounds.
- **SRS-HAL-001:** Each HAL shall advertise its ABI version/size and supported capabilities.
- **SRS-HAL-002:** A HAL shall return a defined NOT_SUPPORTED status for unsupported operations.
- **SRS-HAL-003:** Async request buffers shall remain valid until one terminal completion; cancellation and device removal shall not cause double completion or use-after-free.
- **SRS-PLAT-001:** Each OS binding shall use native enumeration, I/O, synchronization, power, and security APIs only in its platform layer.
- **SRS-PLAT-002:** Each binding shall map native failures to documented core/HAL statuses and preserve useful diagnostics.
- **SRS-TEST-001:** Host tests shall cover valid PING/ECHO, unsupported operation, malformed length, output-capacity failure, and C++ linkage.
- **SRS-TEST-002:** Platform integration tests shall identify OS build, compiler/SDK, device profile, and hardware/firmware revision.
- **SRS-DOC-001:** Each device profile shall list missing inputs and shall not claim physical compatibility until accepted against a supplied specification.

## 3. Quality attributes
- **Memory:** Shared message handling does not allocate memory; platform bindings document any allocation and exhaustion behavior.
- **Concurrency:** Each platform defines synchronization, queue depth, ordering, timeout, cancellation, and completion rules.
- **Reliability:** Errors are propagated; success is never synthesized for unsupported or failed operations.
- **Portability:** Shared source uses the documented portable C subset and is built with strict warnings on host toolchains.
- **Observability:** Native bindings provide traceable status/error events without logging secrets or sensitive device payloads.
- **Maintainability:** Protocol changes update versioning, tests, all binding compatibility notes, and training material.

## 4. Security and safety requirements
- **SRS-SEC-001:** Validate user-controlled lengths, IDs, operation values, and buffer capacities before dispatch.
- **SRS-SEC-002:** Apply least-privilege device-node/interface permissions and never expose unrestricted raw memory/register access to untrusted clients.
- **SRS-SEC-003:** Gate destructive block/device operations behind explicit test authorization and disposable resources.
- **SRS-SEC-004:** Protect device firmware update/reset paths with authenticity, rollback, and recovery requirements where applicable; no generic policy is assumed.
- **SRS-SAFE-001:** Define safe-state behavior, watchdog/interlock behavior, and fault response for every actuator/robot/critical device profile before implementation.
- **SRS-SAFE-002:** A documentation or simulator pass shall not be represented as safety certification or hardware validation.
- **SRS-SAFE-003:** Each safety-relevant deployment shall maintain a version-controlled profile identifying intended use, exact configuration, domains/jurisdictions, standards and editions, system boundary, safety authority, independent reviewer, and evidence references.
- **SRS-SAFE-004:** The product-level default operating mode shall be monitor-only. Monitoring shall define data quality, freshness, loss/staleness, alarm ownership, timing limits, and operator response; monitoring status shall not imply that a safety function is satisfied.
- **SRS-SAFE-005:** Any control function shall remain unavailable unless the product-specific design implements and verifies authorization, interlocks, safe-state behavior, independent protection, fault response, and required approvals. Missing, invalid, stale, or mismatched approval/configuration data shall fail closed. A profile flag or HAL capability alone shall never enable control.
- **SRS-SAFE-006:** Reuse of shared protocol/HAL code shall not inherit or transfer another product's risk classification, safety case, assurance level, regulatory approval, or certification.
- **SRS-SAFE-007:** Product hazard and security assessments shall record unresolved assumptions, severity/risk method, evidence, owners, residual risk, independent review, and revalidation triggers; unresolved release-blocking items prevent safety-related deployment.

## 5. Acceptance tests
| Requirement | Minimum acceptance evidence |
|---|---|
| SRS-CORE-001–004 | Strict C/C++ host build and boundary/negative unit tests; sanitizer run where available |
| SRS-HAL-001–003 | ABI review and per-binding start/submit/cancel/reset/completion tests |
| SRS-PLAT-001–002 | Native SDK build, API compliance checks, error-path tests and lifecycle trace |
| SRS-TEST-001–002 | Repeatable test logs with toolchain, OS, profile, and target identity |
| SRS-SEC-001–004 | Threat review, access-control tests, fuzzing and destructive-action review |
| SRS-SAFE-001–002 | Approved hazard analysis, traceability, independent review and target evidence |
| SRS-SAFE-003–007 | Validated deployment profile, product-specific hazard/threat analyses, requirements trace, reviewed safety case, authorization/safe-state verification, target/HIL evidence, independent approval |

## 6. Deferred requirements
Native Android, macOS, Linux-kernel, and FreeRTOS builds remain deferred until their SDK/toolchain/headers and exact target are available. Device-specific transport behavior remains deferred until the relevant hardware specification is supplied.
