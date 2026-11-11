# Cross-platform driver portfolio

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

This repository is a **starter portfolio**, not one universal driver and not a production or safety-certified product. Portable protocol logic is shared only where the actual device protocol is identical; bus and OS behavior belongs in separate bindings.

## Current reference protocol
`Shared/` implements a test-only PING/ECHO protocol and HAL contract, with a deterministic synchronous simulator backend and an in-memory transactional-store model. These reference models do not describe or validate any physical device. The Windows UMDF project routes one IOCTL through the core. Linux, Android, macOS, and FreeRTOS folders contain distinct reference bindings with different verification status. Device profiles are requirements templates until a physical device specification is supplied.

## Quick checks
```bash
make -C Shared clean test
make -C Linux user-client
python -m unittest discover -s DriverTest/tests -v
```

Build the Windows project with the WDK in Visual Studio (`UMDF 2 Driver1/UMDF 2 Driver1.vcxproj`, Debug x64). Run signed experimental drivers only in a disposable VM. Never expose a host physical disk to destructive tests.

## Documentation
- `AGENTS.md`, `llms.md`, `conversation.md`: contributor/assistant rules and durable decisions.
- `docs/requirements/MRD-market.md`, `MRD-master.md`, `SRS.md`: requirements baseline.
- `docs/architecture.md`, `docs/engineering-guide.md`, `docs/platform-matrix.md`, `docs/platform-extension-guide.md`: system boundaries and development.
- `docs/test-plan.md`, `docs/device-modeling.md`, `docs/critical-systems-validation.md`: test and validation process.
- `docs/safety/`: domain screening, per-deployment profile schema, preliminary reference assessment, hazard/security, safety-case and independent-review templates. This is not product approval or certification.
- `DriverTest/models/device-dossier.schema.json`, `device-dossier.template.json`, `device-spec.schema.json`: machine-readable intake/specification baseline; `tools/driver_factory/` validates and generates evidence-package drafts.
- `docs/target-matrix.md`, `docs/device-dossier-automation.md`, `docs/reuse-inventory-driver-factory-hil.md`: target maturity, safe dossier collection, and external factory inventory.
- `docs/adapter-contract.md`, `DriverTest/models/adapter-capabilities.json`: native adapter boundary markers and evidence/maturity inventory.
- `DriverTest/qemu/run-ephemeral-guest.sh`, `docs/qemu-ephemeral-testing.md`: user-supplied, checksum-verified, disposable QEMU guest workflow (does not download OS media).
- `.github/workflows/factory-ci.yml`, `notebooks/`, `docs/static-site.md`: verified portable CI and static GitHub Pages/notebook export; unavailable assurance/SDK tools remain open gates.
- `docs/simulators/reference-nvram.md`: C11 transactional-store reference model behavior and limitations.
- `Shared/include/device_protocol_sim.h`, `docs/adapter-contract.md`: deterministic HAL-backed protocol simulator; synchronous completion and virtual fault injection are reference behavior only.
- `docs/training/`: staged curriculum and assessment.
- `docs/roadmap.md`: implementation and verification status.
- `docs/license-and-provenance.md`, `NOTICE.md`, `LICENSE`: licensing and provenance rules.

## Support status
See `docs/platform-matrix.md` and `docs/roadmap.md`. Native Android, macOS, FreeRTOS and Linux-kernel builds are not verified in the current Windows/WSL environment. No specific physical-device implementation is claimed.

Critical-systems readiness is not established for any product. The repository's PING/ECHO UMDF sample is non-safety development/training code. Per-deployment hazard/security analysis, standards/edition approval, target/HIL testing, and independent assessment remain required before any safety-related use. Monitoring is not automatically safe; control must remain disabled until product-specific safeguards and approvals are verified.
