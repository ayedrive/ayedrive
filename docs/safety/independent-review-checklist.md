# Independent review and release gate

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

Use the independence and competence criteria required by the selected domain standard, regulator, customer, and assurance route. A checkbox is not an approval; preserve named reviewers, rationale, signed records, findings, and closure evidence.

## Reviewer qualification and independence
- Reviewer identity, role, organization, domain competence, relevant training/experience: recorded.
- Independence from authorship, implementation, and verification assessed against applicable requirements; conflicts disclosed and managed.
- Scope, configuration, artifacts, criteria, evidence access, and review methods agreed before review.
- Reviewer has authority and time to raise blockers and require remediation; no self-approval where independence is required.

## Review evidence
- Intended-use boundary, hazards, risk controls, requirements allocation and traceability.
- Shared core/HAL assumptions, ABI/versioning, capabilities, buffer ownership, failure semantics and limits of reuse.
- Platform-native implementation, OS lifecycle, cancellation/removal/power/reset behavior, permissions, DMA/interrupt handling.
- Security threat assessment and safety/security interaction, update/recovery, diagnostics and failure containment.
- Verification strategy, independence, coverage, fault injection, HIL realism, test equipment calibration and reproducibility.
- Toolchain/dependencies/provenance, configuration/change control, release signing and rollback.
- Operator training, procedures, monitoring, incident response, maintenance and residual-risk acceptance.

## Findings and closure
| Finding ID | Severity/priority rationale | Requirement/claim affected | Owner | Corrective action and objective closure evidence | Reviewer/closure date |
|---|---|---|---|---|---|
| TBD | TBD | TBD | TBD | TBD | TBD |

## Decision record
- Configuration hash/revision and scope reviewed: TBD
- Unresolved findings and accepted deviations: TBD
- Decision: `approved`, `approved_with_constraints`, or `rejected` (default: **rejected until signed**)
- Decision authority, reviewer signatures, dates, expiry/review triggers: TBD
- No approval can establish certification unless the relevant formal assessment/certification process has actually been completed by the authorized parties.
