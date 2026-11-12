# AyeDrive product contract

SPDX-License-Identifier: GPL-3.0-only
Copyright (c) 1993-2026 Abhishek Choudhary. All rights reserved.

## Coverage

AyeDrive defaults to maximum and strictest coverage. The device coverage contract enumerates transport, HID, media, graphics, imaging, buses, sensors, power, security, wireless, storage, virtualization, identity, safety/medical, and multifunction/hotplug families. Reducing scope requires an explicit reviewed decision and compensating tests.

## Verification

Every family has a behavioral AyeAI AyeAM simulation and is exercised through a black-box harness independent of simulator implementation internals. `scripts/ayeadrive-e2e.sh` consumes the repository specification and produces reproducible JSON and Markdown audit artifacts. Recoverable/non-critical failures are logged and do not stop the default run; `AYEADRIVE_FAIL_FAST=1` enables strict release behavior.

## Evidence boundary

The product contract does not claim physical compatibility, HIL, safety approval, accreditation, regulatory acceptance, or certification. Those require exact target hardware, applicable jurisdiction and standards, qualified independent review, and authorized evidence.
