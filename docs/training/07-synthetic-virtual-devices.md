# AyeDrive synthetic device laboratory

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Learning objectives

- Map all AyeDrive device families to explicit behavioral operations, capacities, and failure modes.
- Run independently launched `AyeAI: AyeAM <device> sim` realizations through the black-box harness.
- Exercise a pure C11 model behind the repository's versioned HAL without kernel or hardware dependencies.
- Distinguish model verification from device compatibility, guest integration, HIL, and product safety approval.

## Contract and boundaries

The contracts in `DriverTest/models/device-coverage-contract.json` and the realizations in `tools/ayeadrive_sim/` cover transport, HID, media, graphics, imaging, buses, sensors, power, security, wireless, storage, virtualization, identity, safety/medical, and multifunction/hotplug behavior. They are synthetic behavioral models, not hardware dossiers or vendor emulators.

## Build and run

From the repository root, run `make -C Shared clean test`. The `test_device_virtual_model` suite checks each profile, bounds, round-trips, deterministic packet drops, stream underruns, virtual watchdog expiry, DMA alignment, HAL completion, and time behavior. The tests use strict C11 warnings in CI.

Run the independent AyeDrive process with `bash scripts/ayeadrive-e2e.sh artifacts/ayeadrive`. It writes JSON and Markdown audit reports and continues after recoverable failures by default.

To repeat the portable model/client checks inside a Linux guest, mount or copy the repository into the guest and run `bash Shared/tests/test_virtual_models_in_guest.sh`. This builds the model tests and Linux userspace client. The client can exercise a live ioctl only after `Linux/kernel/dpc_reference.c` has been built against the exact guest kernel, loaded in that disposable guest, and exposed as `/dev/dpc-reference`. Do not substitute host-device passthrough.

## Exercises

1. Add a test proving that reset clears one profile's queue/state but preserves virtual time.
2. Add a fault case and assert both the returned HAL status and instrumentation counter.
3. Design a synthetic golden trace for two interacting profiles. State which timing assumptions are modeled and which are not.
4. Extend the dossier template with provenance/review data. Keep every unknown field explicitly unknown; do not promote a synthetic profile to `accepted_for_modeling`.
5. Explain why successful model tests do not establish cache-coherent DMA, real-time guarantees, actuator safe state, hardware compatibility, or safety approval.

## Evidence interpretation

Passing tests establish deterministic behavior of this implementation under the tested C compiler and inputs. Guest execution adds an OS/build-environment observation only if the guest boot and test output are recorded. Neither result is physical HIL, formal proof, independent review, certification, or authorization to control equipment.
