# Safety case template

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

A safety case is a controlled, product-specific argument supported by reviewable evidence. This template is not itself a safety case or approval.

## Document control
- Product/profile/configuration and exact build hash: TBD
- Intended use, operating domain, jurisdictions, safety authority: TBD
- Applicable standards, editions, scope decisions and assessor: TBD
- Revision, authors, independent reviewer(s), approvals and dates: TBD

## Top-level claim
**C0:** The identified product, in its stated configuration and intended operating context, is acceptably safe for the stated use subject to the listed assumptions and operational constraints. **Status: unsubstantiated until all claims below have evidence and approvals.**

## Context and assumptions
- C1 System boundary, interfaces, users, environment, assumptions and exclusions are complete: TBD evidence.
- C2 Applicable standards/regulatory/customer obligations and assessment route are formally determined: TBD evidence.
- C3 Safety functions, operating modes, safe states, timing constraints and independence requirements are defined: TBD evidence.

## Subclaims and evidence
| Claim | Argument / rationale | Required evidence | Evidence identifier, revision, owner, reviewer, status |
|---|---|---|---|
| C4 Hazards are systematically identified and risks reduced/accepted by authority | TBD | Approved hazard analysis, risk controls, traceability, residual-risk acceptance | TBD |
| C5 Requirements are complete, allocated, consistent and verified | TBD | Requirements baseline, architecture allocation, review and verification records | TBD |
| C6 Shared core/HAL and native binding are suitable for the allocated safety role | TBD | Safety architecture, interface contracts, failure analysis, platform-specific evidence | TBD |
| C7 Faults, timing, concurrency, power, reset, communication and degraded modes are controlled | TBD | Analysis, deterministic fault injection, target tests, recovery evidence | TBD |
| C8 Cybersecurity risks do not defeat safety controls or safe states | TBD | Threat model, security requirements/tests, update and vulnerability processes | TBD |
| C9 Toolchain, dependencies, configuration and release are controlled | TBD | Reproducible build, provenance/SBOM, tool records, change control, signing/recovery | TBD |
| C10 Independent assessment and operational controls are adequate | TBD | Competence/independence record, audit findings, training, maintenance/monitoring plan | TBD |

## Defeaters, limitations and residual risks
List unresolved assumptions, anomalous conditions, known limitations, deviations, accepted residual risks, owners, expiry/review dates, and constraints that invalidate the argument. TBD.

## Conclusion and authority decision
- Claim status: **OPEN — NOT APPROVED**
- Decision authority and basis: TBD
- Independent assessor/reviewer statement and signed record: TBD
- Approved configuration and operating restrictions: TBD
- Revalidation triggers: hardware/firmware/OS/toolchain/HAL/protocol changes, incidents, new threats, changed intended use, or standards/regulatory updates.
