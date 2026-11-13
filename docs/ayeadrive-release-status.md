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
- The C display simulator is included in `make -C Shared test` with mode, framebuffer, fence, fault, reset, and backend-capability checks.
- Linux display execution has two explicit modes: `make -C Shared display-linux-x11` is visible by default through X11/WSLg, while `make -C Shared test-display-linux-headless` is the opt-in automation path.
- The QEMU HAL display vertical slice is implemented with an explicit virtio-serial transport, guest `display_guest` client, and host `display_x11_bridge.py`; native and guest-model validation pass, but QEMU E2E visual execution remains unverified because no authorized guest ISO/checksum is present locally.
- The seeded Windows `DriverTest/qemu/run-windows-guest.sh` path now uses a visible SDL window by default; `QEMU_DISPLAY=gtk` selects GTK and `QEMU_HEADLESS=1` selects `-display none`. The real Windows guest was not launched in this validation because no authorized installed Windows qcow2/VHDX and no OVMF firmware were available; Ubuntu ISO media and DriverFactory overlay qcow2 files are not substitutes.
- A real Linux QEMU guest display path was validated: the guest kernel/initramfs ran the static C HAL client, emitted a 640x480x4 framebuffer through virtio-serial, and `tools/display_fanout.py` delivered the same frame to the WSL/X11 window and Windows browser endpoint. This is Linux guest evidence, not Windows-driver guest evidence.
- Integrated `ci-local.ps1` passes.
- Bash workflow passes under WSL after LF normalization and emits audit artifacts.
- Standalone HAL harness passes all 15 families with lifecycle, submit, cancel, reset, logical-time, fault, and family-exercise evidence.
- Standalone HAL/OS matrix validates for five declared targets: Windows UMDF, Linux, Android, macOS DriverKit, and FreeRTOS.
- Cross-platform QEMU runner validates and emits a prerequisite-gated emulation report without attaching host disks or passthrough devices.

## Explicitly not completed

This release-ready local scope does not establish:

- Any vendor-specific physical device compatibility
- Native Windows UMDF/KMDF, Linux kernel, Android, macOS, or RTOS production support
- Physical HIL, EMC/electrical, RF, thermal, clinical, or safety testing
- Independent technical review or process audit execution
- Accredited laboratory evidence
- Regulator/customer acceptance or jurisdiction-specific conformity
- Safety certification, security certification, SIL/ASIL, or medical approval
- OS guest conformance through QEMU: the runner is ready, but no guest result is claimed until authorized media, an expected checksum, and a guest-specific test command are supplied.
- Interactive Linux/X11 display presentation through QEMU is not claimed: the existing seeded QEMU evidence is headless and does not provide a guest X display or display-driver integration result.
- Native macOS DriverKit and FreeRTOS target builds: source starters remain separate from SDK/toolchain and target validation.

Publishing to `https://github.com/ayedrive/ayedrive` remains a separate authorized release operation. No push or remote modification was performed.
