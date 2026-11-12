# AyeDrive release version policy

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

AyeDrive uses semantic `x.y.z` versions:

- **Major (`X.0.0`)**: incompatible changes to public simulator protocols, HAL contracts, machine-readable schemas, release artifact interfaces, or documented automation contracts.
- **Minor (`0.X.0`)**: backward-compatible device-family additions, simulator capabilities, assurance/test features, platform bindings, or user-facing functionality.
- **Patch (`0.0.X`)**: backward-compatible defect fixes, validation corrections, documentation corrections, provenance updates, and test-only changes that do not alter the public contract.

Pre-1.0 development releases may still change rapidly. Version `0.1.0` is the initial first-deposition development release for the AyeDrive repository. A release must have a clean, reviewed source revision, reproducible validation results, an exact artifact checksum, and an evidence/provenance record. A Zenodo version must match the Git release version exactly.

Release metadata must not claim physical compatibility, certification, accreditation, regulator acceptance, safety approval, or production readiness unless the corresponding authorized evidence exists.
