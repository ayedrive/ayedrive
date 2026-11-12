# Certification-readiness and evidence status

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Status

**Formal certification: not claimed.**  
**Accreditation: not claimed.**  
**Regulator/customer approval: not claimed.**

The repository now contains an automated planning and traceability contract. It is a foundation for an assurance process, not the process outcome.

## Evidence matrix

| Area | Automated record | Current status | Missing evidence |
|---|---|---|---|
| Product requirements | `DriverTest/models/assurance-package.json` | Planning package validated | Authorized baseline, risk classification, acceptance criteria |
| Target hardware | `targetHardware` records | Abstract assumptions | Vendor/model/revision/firmware, schematics, interface and errata evidence |
| Six simulator profiles | `simulators` records and `Shared/core/device_virtual_model.c` | Host simulation observed | Physical model correlation, hardware behavior, HIL |
| Portable HAL/protocol | Strict C/C++ tests | Passed | Target ABI, OS integration, timing and deployment evidence |
| Windows UMDF | `HW-WINDOWS-UMDF` | Assumption | Windows SDK/build, signed driver package, device tests, WHQL/customer criteria |
| Linux | `HW-LINUX-REFERENCE` | Userspace compile observed | Matching kernel headers, module build/load, `/dev` behavior, target hardware |
| Android | `HW-ANDROID-AIDL` | Planning only | Android SDK/NDK/BSP/device, AIDL build, CTS/VTS or customer criteria |
| Independent technical review | `REV-INDEPENDENT-TECHNICAL` | Planned/open | Qualified independent reviewer, conflict declaration, signed report |
| Process audit | `REV-PROCESS-AUDIT` | Planned/open | Configuration management, tool qualification, defect/change records, audit report |
| Accredited laboratory | `REV-ACCREDITED-LAB` | Planned/open | Applicable accreditation scheme, scope, contract, witnessed results |
| Standards/regulators | `standardsAssumptions` | Applicability assumptions | Jurisdiction, intended use, risk class, confirmed standards and regulator interpretation |
| Reproducibility | schema/CLI/attestation tooling | Host workflow observed | Controlled release archive, signed artifacts, independent reproduction |

## Automated gates

The following commands are executable gates:

```powershell
python tools/driver_factory/validate_schemas.py
$env:PYTHONPATH = "$(Get-Location);$(Join-Path (Get-Location) 'tools')"
python -m driver_factory assurance-validate DriverTest/models/assurance-package.json
python -m unittest discover -s tools/driver_factory/tests -v
```

The assurance validator rejects certification or approval claims, unresolved traceability references, duplicate identifiers, missing review gates, and a formal `approved` package status.

## Required progression before any certification claim

1. Establish intended use, product boundary, hazards, jurisdiction, and applicable regulator/customer requirements.
2. Replace all abstract hardware records with authorized, revision-controlled target identities.
3. Complete architecture, threat, safety, and risk analyses appropriate to the product.
4. Qualify or justify all tools and test environments used for assurance evidence.
5. Execute target OS, driver, hardware, HIL, EMC/electrical, cybersecurity, and safety tests required by the confirmed scope.
6. Obtain independent review from a competent party with documented independence.
7. Use an accredited or regulator-accepted laboratory when required and retain its scope and witnessed evidence.
8. Resolve all deviations and obtain written approval from the responsible authority.

No item above is satisfied merely by passing the synthetic simulator tests.
