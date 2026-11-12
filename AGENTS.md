# AyeDrive agent instructions

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Project intent

AyeDrive is an automated end-to-end device driver ecosystem by AyeAI. Default scope is maximum and strictest coverage across every requested device family. Do not narrow coverage unless an explicit scope decision records the rationale, owner, affected requirements, and compensating tests.

This repository is a portfolio of independent device-family and operating-system bindings around a portable protocol core/HAL. Shared code is appropriate only for protocol/state-machine behavior that is identical across targets; bus enumeration, kernel APIs, DMA, interrupts, power, permissions, cancellation, and vendor frameworks stay in platform bindings.

## Required practices

- Preserve the GPL-3.0-only identifiers and copyright notice on repository-origin
