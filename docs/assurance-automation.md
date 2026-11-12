# Requirements-to-evidence automation

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Boundary

`DriverTest/models/assurance-package.json` is a planning and traceability package. It does **not** certify the product, establish accreditation, prove regulatory compliance, approve a safety function, or establish hardware compatibility. Every unresolved item remains explicitly `planned` or `assumption`.

The package is intentionally conservative: exact hardware identity, intended use, jurisdiction, risk class, applicable standards, independent reviewer, accredited laboratory, and approval authority are inputs that must be supplied by an authorized product owner. The repository does not infer those facts.

## Automated contract

The package schema defines:

- product requirements with risk, verification method, status, and evidence references;
- abstract target hardware records for Windows UMDF, Linux, and Android;
- six synthetic simulator mappings;
- independent technical, process-audit, and accredited-laboratory review gates;
- configuration, independence, and accreditation assumptions;
- jurisdiction and standards applicability assumptions requiring confirmation;
- executable test-case records with expected evidence.

Validate it with:

```text
python tools/driver_factory/validate_schemas.py
python -m driver_factory assurance-validate DriverTest/models/assurance-package.json
```

The semantic validator rejects unknown requirement references, duplicate identifiers, missing independent-review gates, certification/approval claims in the boundary, and `approved` package status. It emits `certification: not_claimed` on success.

## Evidence lifecycle

1. Baseline requirements and intended use with an authorized product owner.
2. Replace abstract hardware identities with traceable vendor/model/revision/firmware records.
3. Confirm jurisdiction, regulator, customer contract, risk class, and applicable standards.
4. Execute simulator and host tests; retain toolchain, source, input, output, and hash metadata.
5. Perform independent technical and safety/process reviews with conflict and competence records.
6. Use an accredited laboratory only when required, and verify that its accreditation scope covers the exact activity.
7. Resolve findings and deviations; do not convert `observed` simulation into hardware or certification evidence.
8. Obtain formal approval only from the responsible authority through its accepted process.

## Current evidence interpretation

The six virtual profiles and strict C/C++ tests establish deterministic behavior of repository-owned software models. They do not establish physical-device behavior, native Windows UMDF compatibility, Linux kernel-module compatibility, Android HAL compatibility, HIL, safety, regulator acceptance, accreditation, or certification.
