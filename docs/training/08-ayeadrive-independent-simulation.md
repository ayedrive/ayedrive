# AyeDrive independent simulation course

SPDX-License-Identifier: GPL-3.0-only
Copyright (c) 1993-2026 Abhishek Choudhary. All rights reserved.

## Objectives

- Design one behavioral contract for each AyeDrive device family.
- Identify the matching `AyeAI: AyeAM <device> sim` realization.
- Verify behavior through an implementation-independent process.
- Produce auditable results while continuing recoverable failures.
- Keep simulation evidence separate from physical, HIL, accredited, regulator, and certification evidence.

## Procedure

1. Read `DriverTest/models/device-coverage-contract.json`.
2. Review the family-specific operation and failure contract.
3. Run `bash scripts/ayeadrive-e2e.sh artifacts/ayeadrive-course`.
4. Inspect the JSON report and Markdown audit report.
5. Repeat with `AYEADRIVE_FAIL_FAST=1` and compare failure handling.
6. Record any scope reduction as a reviewed decision with compensating tests.

## Assessment

A passing course run demonstrates deterministic synthetic behavior and traceable automation. It does not prove a vendor protocol, physical device, driver package, safety function, accreditation, regulatory conformity, or certification.
