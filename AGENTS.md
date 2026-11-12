# Repository agent instructions

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Project intent

AyeDrive is the product. Default scope is maximum and strictest coverage across every requested device family. Do not narrow coverage unless an explicit scope decision records the rationale, owner, affected requirements, and compensating tests.
This repository is a portfolio of independent device-family and operating-system bindings around a portable protocol core/HAL. It is not one universal driver. Shared code is appropriate only for protocol/state-machine behavior that is identical across targets; bus enumeration, kernel APIs, DMA, interrupts, power, permissions, cancellation, and vendor frameworks stay in platform bindings.

## Required practices
- Preserve the GPL-3.0-only identifiers and copyright notice on repository-original files. Do not claim ownership of SDK headers, vendor code, generated output, or third-party examples. Keep provenance and license review visible.
- Label the PING/ECHO implementation as a reference/test protocol, never as a physical device protocol.
- Keep each OS and device family separate. Do not claim a profile is an emulator, driver, supported hardware, or production-ready unless its implementation and tests prove that claim.
- Validate lengths, integer overflow, request ownership, cancellation, access rights, and error mapping at every boundary. Do not add unchecked kernel/user copies or unsafe write tests.
- Avoid dynamic allocation in shared protocol parsing. Preserve bounded buffers and add tests for malformed lengths, unsupported versions/operations, and insufficient output capacity.
- For every code change, add or update tests, run the available target build, and state which SDKs/hardware were unavailable. Do not state that native Android/macOS/FreeRTOS/Linux-kernel validation passed unless it actually ran.
- Safety-related claims require requirements traceability, hazard analysis, independent review, target evidence, and an identified domain standard. Documentation is not certification.
- Do not initialize a Git remote, publish, or push. User requested local Git only. Do not invent author identity.
- Prefer existing standard libraries and toolchains; justify new dependencies. Never put secrets, private signing keys, or production device credentials in the repository.
- Implement behavioral AyeAI-branded realizations named `AyeAI: AyeAM <device> sim`, not metadata-only category entries.
- Validate simulations with the implementation-independent AyeDrive black-box harness. Normal automation continues after recoverable/non-critical failures and records them; release gates may use fail-fast mode.
- Every end-to-end run must emit auditable JSON and Markdown results. Simulation evidence never substitutes for physical, HIL, independent-review, accredited-lab, regulator, or certification evidence.

## Source of truth
Read `conversation.md`, `llms.md`, `docs/requirements/`, `docs/architecture.md`, `docs/platform-matrix.md`, and `docs/critical-systems-validation.md` before changing shared contracts or claiming platform support.
