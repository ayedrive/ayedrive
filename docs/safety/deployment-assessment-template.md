# Product deployment assessment template

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

Copy this file per controlled product/use case. Replace every `TBD`; unresolved items are release blockers. Link controlled records rather than embedding sensitive operational details in this repository.

## A. Configuration and intended use
- Profile ID/version and change owner: TBD
- Product, hardware IDs/revisions, firmware, platform/OS, compiler/toolchain: TBD
- Intended use, prohibited use, users/operators, operating environment and lifecycle: TBD
- Deployment geography, regulator, customer requirements and applicable editions: TBD
- System boundary, interfaces, assumptions, external dependencies and responsibility allocation: TBD
- Domain profile(s): TBD; domain is not a safety classification

## B. Operating modes and authority
- Default mode: **monitor-only**
- Is control present? Who can enable it, by what authenticated/authorized means, and under what interlocks? TBD
- State machine, transition guards, startup/restart/update behavior, and mode indication: TBD
- Safe state for each failure and how it is reached/verified: TBD
- Independent protection layers, emergency stop/override, watchdogs, and manual operation: TBD
- What happens on loss, corruption, delay, replay, disagreement, or uncertainty in data/commands? TBD
- Control enablement approval and configuration integrity mechanism: TBD

## C. Hazard and risk analysis
Record method, assumptions, participants, date, revision, and controlled evidence link. Use the domain-appropriate method; do not copy severity/probability ratings across domains without justification.

| Hazard ID | Hazardous situation / cause | Consequence | Existing safeguards | Required risk reduction | Safety requirement IDs | Owner / evidence |
|---|---|---|---|---|---|---|
| TBD | TBD | TBD | TBD | TBD | TBD | TBD |

Analyze at minimum: incorrect/stale observation; unintended or failed control; malformed input; timing/queue exhaustion; cancellation/removal/reset; power/clock loss; common-cause software/platform faults; update/rollback; human factors; cybersecurity-induced unsafe behavior; maintenance and foreseeable misuse.

## D. Security assessment
- Assets, trust boundaries, attacker capabilities, interfaces, and threat-analysis method: TBD
- Spoofing, tampering, repudiation, information disclosure, denial of service, privilege escalation, update/supply-chain abuse, and physical access: TBD
- Security controls and how failures could affect safety: TBD
- Secure boot/update, key lifecycle, diagnostics, recovery, logging/privacy, vulnerability response: TBD
- Residual threats, compensating controls, owner and approval: TBD

## E. Safety requirements and traceability
Each requirement must be measurable, uniquely identified, allocated to a component/role, and linked to verification and validation evidence.

| Safety requirement ID | Requirement and rationale | Allocation | Failure response / safe state | Verification method and pass criterion | Evidence ID/status |
|---|---|---|---|---|---|
| TBD | TBD | TBD | TBD | TBD | TBD |

## F. Standards, assurance and assessment route
| Standard/regulation/customer requirement | Edition/jurisdiction | Applicable scope and rationale | Required lifecycle/assurance objectives | Competent approver and evidence |
|---|---|---|---|---|
| TBD | TBD | TBD | TBD | TBD |

Do not infer SIL/ASIL/DAL/PL/class from a domain name. Record formal allocation/assessment only where required and approved by the responsible authority.

## G. Verification and validation plan
- Requirements-based tests, unit/integration/system, robustness/fuzzing, static analysis, concurrency, timing/WCET as applicable: TBD
- Fault injection, negative tests, power loss, reset, watchdog, removal, communication degradation and recovery: TBD
- Simulator/VM fidelity limits, exact target hardware/firmware, HIL fixture calibration and test safety controls: TBD
- Independence of verification, coverage/closure criteria, deviations and regression impact rules: TBD
- Tool qualification/validation rationale where required; exact versions and reproducibility: TBD

## H. Approval and residual risk
- Safety authority and organization: TBD
- Independent reviewer(s), competence, independence rationale, conflicts and signed review record: TBD
- Residual risk statement, assumptions, operational controls and acceptance authority: TBD
- Release configuration/hash, approved mode, restrictions, rollback/recovery and monitoring: TBD
- Decision: **NOT APPROVED** until every required gate is signed and evidence is controlled.
