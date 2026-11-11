# Deployment safety profiles

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

Each product deployment requires its own controlled safety profile, even when portable protocol code or a HAL implementation is reused. Domain names are configuration selectors for review checklists and evidence plans; they do not assign an integrity level, establish applicability, or imply certification.

## Files
- `deployment-profile.schema.json` — metadata/traceability shape, not an enforcement mechanism.
- `reference-monitor-only.json` — status of the current non-safety UMDF/PING-ECHO reference.
- `domain-applicability-matrix.md` — candidate standards/topics and profile selection prompts.
- `deployment-assessment-template.md` — product-specific scope, hazards, threats, and decisions.
- `safety-case-template.md` — claim/argument/evidence record template.
- `independent-review-checklist.md` — reviewer independence and evidence gate.
- `preliminary-reference-assessment.md` — scoped preliminary assessment of the existing reference sample.

## Mode policy
`monitor_only` is the required default for all profiles. The schema's `control_enablement: approval_gated` means only that a project has recorded the required review references; it does not switch or authorize a runtime control path. An actual product must implement authorization and safe-state behavior in its verified native architecture. If required information, approvals, or protective functions are absent or invalid, control must remain unavailable and the system must enter the product-defined safe state.

A monitor is not automatically safe: telemetry may be stale, wrong, delayed, unauthenticated, or misunderstood. Define thresholds, quality flags, timestamps, loss-of-data behavior, alarm ownership, and operator response for every monitoring use.

No profile in this directory is approved for deployment. Standards and editions must be confirmed by qualified domain, regulatory, and safety authorities for the product, jurisdictions, customers, and lifecycle.
