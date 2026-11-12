# AyeDrive

Automated end-to-end device driver ecosystem by AyeAI.

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Project status

AyeDrive is a starter portfolio and behavioral simulation platform, not one universal driver and not a production or safety-certified product. Portable protocol logic is shared only where the actual device protocol is identical; bus and OS behavior belongs in separate bindings.

The repository includes deterministic `AyeAI: AyeAM <device> sim` realizations for the maximum-coverage device family contract, an implementation-independent black-box harness, requirements/evidence automation, and platform reference bindings. Simulation evidence does not establish physical-device compatibility, HIL, accreditation, regulator acceptance, or certification.

## Quick checks

```bash
make -C Shared clean test
python -m unittest discover -s DriverTest/tests -v
bash scripts/ayeadrive-e2e.sh artifacts/ayeadrive
```

The Bash workflow continues after recoverable/non-critical failures and writes JSON/Markdown audit records. Use `AYEADRIVE_FAIL_FAST=1` for release gates.

## Documentation

- `AGENTS.md`, `llms.md`, `llms.txt`, `conversation.md`, `context.md`, `contract.md`: contributor, assistant, product, and durable decision contracts.
- `docs/ayeadrive-simulation-howto.md`, `docs/ayeadrive-release-status.md`: AyeDrive operation and release status.
- `docs/release-version-policy.md`: x.y.z release-version policy and evidence requirements.
- `docs/architecture.md`, `docs/requirements/`, `docs/platform-matrix.md`: architecture and product requirements.
- `docs/assurance-automation.md`, `docs/certification-evidence-status.md`: requirements-to-evidence automation and evidence boundaries.
- `DriverTest/models/device-coverage-contract.json`, `assurance-package.json`: machine-readable coverage and assurance contracts.
- `tools/ayeadrive_sim/`: independent-process AyeAM behavioral simulations and conformance harness.
- `docs/training/`, `notebooks/`: lessons, courses, and Colab-compatible workflows.

Build the Windows project with the WDK in Visual Studio and run signed experimental drivers only in a disposable VM. Never expose a host physical disk to destructive tests.

## Evidence boundary

Native Android, macOS, FreeRTOS, and Linux-kernel builds, physical HIL, independent review, accredited laboratory evidence, regulator/customer approval, safety/security certification, and production readiness remain separate gates until their evidence is executed and accepted.
