# Cross-platform driver portfolio

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

This repository is a **starter portfolio**, not one universal driver and not a production or safety-certified product. Portable protocol logic is shared only where the actual device protocol is identical; bus and OS behavior belongs in separate bindings.

## Current reference protocol
`Shared/` implements a test-only PING/ECHO protocol and HAL contract. It does not describe any physical device. The Windows UMDF project routes one IOCTL through the core. Linux, Android, macOS, and FreeRTOS folders contain distinct reference bindings with different verification status. Device profiles are requirements templates until a physical device specification is supplied.

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
- `docs/training/`: staged curriculum and assessment.
- `docs/roadmap.md`: implementation and verification status.
- `docs/license-and-provenance.md`, `NOTICE.md`, `LICENSE`: licensing and provenance rules.

## Support status
See `docs/platform-matrix.md` and `docs/roadmap.md`. Native Android, macOS, FreeRTOS and Linux-kernel builds are not verified in the current Windows/WSL environment. No specific physical-device implementation is claimed.
