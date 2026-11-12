# Driver Factory development history and experiment log

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

This log records the work and experiments performed during the local multi-OS emulator and learning-platform expansion. It distinguishes repository evidence from assumptions and does not claim hardware validation, certification, or regulator approval.

## Repository implementation

- Added six deterministic, stateful C11 virtual-device profiles behind the portable HAL:
  `char_control`, `block_storage`, `network_fieldbus`, `continuous_stream`,
  `actuator_motion_timing`, and `bus_controller_bridge`.
- Added the virtual-profile JSON contract and JSON Schema, Python profile checks, strict C model tests, and a compact HAL guest probe.
- Added reset, bounds, overflow, fault-injection, watchdog, queue, stream, DMA, and HAL-completion checks.
- Added the Linux guest test procedure and Linux userspace client compilation path.
- Added a synthetic-device training lesson and notebook.
- Updated the static site generator to include the new lesson and assurance automation page.
- Made notebook site rendering portable: the notebook displays the reproducible C test command instead of executing a platform-specific `make` process during static rendering.
- Added requirements-to-evidence automation in `DriverTest/models/assurance-package.json` and its schema, semantic CLI validation, CI integration, and tests.

## Host validation

- JSON schemas validated, including the assurance package.
- DriverTest Python suite passed: 8 tests.
- Virtual-profile contract suite passed: 1 test.
- Driver-factory suite passed: 16 tests, including assurance traceability and non-certification checks.
- Strict C/C++ suite passed:
  - protocol: 5/5;
  - store: 5/5;
  - simulator protocol: 7/7;
  - six-profile virtual model suite: 10/10;
  - HAL guest probe: `GUEST_SYNTHETIC_HAL_PROBE_PASSED`.
- Static site build passed and generated 19 files.
- The full local CI sequence previously passed with `LOCAL_CI_PASSED` using a process-scoped PowerShell execution-policy override.

## Native Windows QEMU experiments

1. WSL was initially used for Linux-oriented tools. This was corrected after recognizing that Windows-native guest tooling should be preferred on a Windows host.
2. Native QEMU 11.1.0 was installed with winget. `qemu-system-x86_64.exe` and `qemu-img.exe` were verified under `C:\Program Files\qemu`.
3. Ubuntu 24.04 cloud image was downloaded to `%LOCALAPPDATA%\DriverFactory\guest` and matched the publisher checksum:
   `04e8f2461a16a44e78a370284471f29616cdd9baf1893df25807b71f985b6c94`.
4. Default/legacy QEMU TCG configurations reproduced an Ubuntu kernel panic:
   `Kernel panic - not syncing: IO-APIC + timer doesn't work!`.
5. The panic reproduced without any cloud-init seed, proving the seed was not the root cause.
6. `kernel_irqchip=off` and `acpi=off` did not resolve the panic.
7. Single-threaded TCG with `-icount shift=auto` avoided the APIC timer panic and reached Ubuntu system startup/login.
8. A native NoCloud seed ISO was created with Python `pycdlib`. With `-icount shift=auto`, Ubuntu reported:
   `Datasource DataSourceNoCloud [seed=/dev/sr0]` and `cloud-init ... finished`.
9. Loopback SSH forwarding and guest command execution were not completed reliably in the recorded session. Therefore no guest-side model result is claimed here.
10. The guest image, seed, temporary credentials, and QEMU overlays are local test artifacts, not repository evidence and must not be treated as certification artifacts without controlled capture and hashing.

## Evidence boundaries

Synthetic model tests establish deterministic behavior of this implementation under tested compiler/toolchain inputs. They do not establish native Windows UMDF compatibility, Linux kernel-module compatibility, Android HAL compatibility, physical-device behavior, HIL, timing guarantees, safety, accreditation, regulator acceptance, or certification.
