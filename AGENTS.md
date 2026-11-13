# AyeDrive agent instructions

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Project intent

AyeDrive is an automated end-to-end device driver ecosystem by AyeAI. Default scope is maximum and strictest coverage across every requested device family. Do not narrow coverage unless an explicit scope decision records the rationale, owner, affected requirements, and compensating tests.

This repository is a portfolio of independent device-family and operating-system bindings around a portable protocol core/HAL. Shared code is appropriate only for protocol/state-machine behavior that is identical across targets; bus enumeration, kernel APIs, DMA, interrupts, power, permissions, cancellation, and vendor frameworks stay in platform bindings.

## Required practices

- Preserve the GPL-3.0-only identifiers and copyright notice on repository-origin

## Systems and QEMU validation practices

- Use short, bounded terminal commands for systems work. Prefer one operation per command and inspect its result immediately.
- Avoid fragile nested-shell quoting. Use absolute paths or change to the repository directory with a separately verified command.
- Monitor background processes immediately and terminate them deliberately; do not leave QEMU, servers, or test processes unattended.
- Keep host, WSL2, QEMU guest, and browser execution layers distinct. A host-side simulator or browser animation is not evidence that a guest device ran.
- For end-to-end claims, observe the actual guest process, transport connection, frame or test output, and sink output. Record missing prerequisites explicitly instead of substituting unrelated images or artifacts.
- Keep visible execution separate from CI headless execution. Visible mode must be explicit in launch arguments, while headless mode must be an intentional option.
- Reuse seeded validation workflows and preserve their evidence boundaries. Do not create parallel claims that bypass the authoritative driver, HAL, or QEMU path.
- Do not fabricate display frames or test results. Browser and native frontends must consume frames from the device HAL or report a clear connection error.
- Strict QEMU policy: never carry, vendor, modify, or distribute QEMU source. QEMU build scripts may use only the host OS package source mechanism or the official QEMU upstream repository, must pin and validate the version, and must keep external checkouts/build output out of repository content.
- Document scope decisions, blockers, toolchain prerequisites, and observed results in repository documentation without including personal data or offensive language.
