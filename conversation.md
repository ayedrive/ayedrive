# Project decision log

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

This file records durable project decisions, not a verbatim transcript.

## User requirements
- Build a set of driver types; do not combine all hardware into one driver.
- Reuse one source for protocol/state-machine logic where device protocols match, and isolate OS implementation behind HAL/platform bindings.
- Initial platform set: Windows, Linux, Android, macOS, FreeRTOS. Document additional OS/platform extension targets.
- Device categories include USB, Bluetooth, Wi-Fi, PCI/PCIe, storage/block, network, robots/control systems, and hardware accelerators.
- Expanded coverage includes transport, HID, audio/video, GPU/display, cameras/imaging, serial/UART/SPI/I2C/CAN/GPIO, sensors/ADC/DAC, power/battery, secure/TPM/crypto, wireless, storage/RAID/NVMe/filesystems, virtualization/paravirtualization, smart-card/biometric/identity, industrial/medical/safety, and multifunction/hotplug families.
- Product name is AyeDrive. Behavioral synthetic realizations use `AyeAI: AyeAM <device> sim` and are tested by an implementation-independent black-box harness.
- Mark original solutions GPLv3 and include: `Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.` The copyright owner retains ownership and may separately license original work.
- Provide AGENTS/LLM/conversation instructions, market and master requirements, engineering documentation, detailed learning/training, and critical-systems validation guidance.
- Initialize Git locally and make an initial commit; do not choose a remote or publish.
- Default automation runs end-to-end without AI or human intervention, continues after recoverable/non-critical failures, and records complete JSON/Markdown audit artifacts; fail-fast is an explicit release option.

## Architecture decision
Portable protocol logic belongs in `Shared/`; OS, bus, DMA, interrupt, power, security, and lifecycle behavior belong in separate bindings. A generic HAL cannot reproduce vendor-specific hardware. Each physical-device profile must be based on actual IDs, protocol, firmware, timing, power, and error behavior.

## Current reference status
The current PING/ECHO protocol and root-enumerated device are test scaffolding only, not the user's physical-device protocol. Platform starter code and class profiles are not proof of native builds or hardware support. See `docs/platform-matrix.md` and `docs/roadmap.md` for current verification status.

## Open inputs needed for device-specific implementations
- Exact manufacturer/model, vendor/product/subsystem IDs, revision and firmware.
- Bus topology and device protocol specifications, including descriptors/register maps/commands, timeouts, resets, power and failure behavior.
- Target OS versions, boards/BSPs, compilers and SDKs.
- Intended safety domain, hazard analysis owner, applicable standards, and validation evidence requirements.
- Git commit email if local Git has no configured identity.
