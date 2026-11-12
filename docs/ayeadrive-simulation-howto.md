# AyeDrive simulation and audit how-to

SPDX-License-Identifier: GPL-3.0-only  
Copyright (c) 1993-2026 Abhishek Choudhary. All rights reserved.

## Default scope

AyeDrive uses maximum and strictest coverage by default. The coverage contract includes transport, HID, audio/video, GPU/display, camera/imaging, serial/UART/SPI/I2C/CAN/GPIO, sensor/ADC/DAC, power/battery, secure/TPM/crypto, wireless, storage/RAID/NVMe/filesystem, virtualization/paravirtualization, smart-card/biometric/identity, industrial/medical/safety, and multifunction/hotplug families.

Each family has an AyeAI-branded behavioral realization named `AyeAI: AyeAM <device> sim`. These are deterministic software devices with explicit state, lifecycle, boundary, fault, and family-specific exercise behavior.

## Run the complete process

From the repository root:

```bash
bash scripts/ayeadrive-e2e.sh artifacts/ayeadrive
```

The default runner validates schemas, executes the implementation-independent black-box harness, runs Python tests, continues after recoverable/non-critical failures, and writes:

- `run.log`
- `run-summary.json`
- `ayeadrive-simulation-report.json`
- `ayeadrive-simulation-report.md`

For a release gate that stops at the first failure:

```bash
AYEADRIVE_FAIL_FAST=1 bash scripts/ayeadrive-e2e.sh artifacts/ayeadrive-release
```

The harness launches each simulator as an independent process and communicates through JSON-lines requests. It does not import simulator implementation classes to decide whether behavior passed.

## Specification-driven operation

The machine-readable coverage and assurance inputs are:

- `DriverTest/models/device-coverage-contract.json`
- `DriverTest/models/assurance-package.json`
- `DriverTest/models/assurance-package.schema.json`

A future device-specific specification must add exact identity, protocol, firmware, timing, power, security, and failure contracts before any physical-device claim. A simulator result is observed simulation evidence only.

## Audit and regulatory boundary

The generated reports include timestamps, family names, expected and observed statuses, branded realization names, and certification status. They do not establish hardware compatibility, HIL, independent review, accredited laboratory evidence, regulator acceptance, safety approval, or certification. Those remain explicit gated evidence activities.
