# AyeDrive release status

SPDX-License-Identifier: GPL-3.0-only  
Copyright (c) 1993-2026 Abhishek Choudhary. All rights reserved.

## Release-ready local scope

AyeDrive now has 15 executable AyeAM behavioral simulation families:

1. Transport: USB, PCIe, Thunderbolt, and related transport behavior
2. HID input
3. Audio/video
4. GPU/display
5. Camera/imaging
6. Serial/UART/SPI/I2C/CAN/GPIO buses
7. Sensors/ADC/DAC
8. Power/battery
9. Secure elements/TPM/cryptographic operations
10. Wireless: Wi-Fi, Bluetooth, cellular
11. Storage controllers/RAID/NVMe/filesystems
12. Virtualization/paravirtualization
13. Smart-card/biometric/identity
14. Industrial/medical/safety observation and alarm behavior
15. Multifunction/hotplug

Each is named `AyeAI: AyeAM <device> sim`, launched independently, exercised through JSON-lines, and included in the audit report.

## Verified locally

- Coverage and assurance schemas validate.
- Assurance package validates as AyeDrive with certification `not_claimed`.
- Independent harness passes all 15 families.
- AyeDrive harness tests pass.
- Driver-factory tests pass.
- Static site and nine notebooks build.
- Strict C/C++ Shared tests pass.
- Integrated `ci-local.ps1` passes.
- Bash workflow passes under WSL after LF normalization and emits audit artifacts.

## Explicitly not completed

This release-ready local scope does not establish:

- Any vendor-specific physical device compatibility
- Native Windows UMDF/KMDF, Linux kernel, Android, macOS, or RTOS production support
- Physical HIL, EMC/electrical, RF, thermal, clinical, or safety testing
- Independent technical review or process audit execution
- Accredited laboratory evidence
- Regulator/customer acceptance or jurisdiction-specific conformity
- Safety certification, security certification, SIL/ASIL, or medical approval

Publishing to `https://github.com/ayedrive/ayedrive` remains a separate authorized release operation. No push or remote modification was performed.
