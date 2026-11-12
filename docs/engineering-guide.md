# Engineering guide

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Repository layout
- `Shared/`: portable C reference protocol, HAL ABI and host tests.
- `UMDF 2 Driver1/`: Windows UMDF software-enumerated reference adapter.
- `Linux/`: Linux reference module and userspace client.
- `Android/` and root `Android.bp`: AIDL NDK service starter.
- `MacOS/`: protocol bridge only; native DriverKit target is outstanding.
- `FreeRTOS/`: static task/queue starter.
- `DriverTest/`: Windows device-interface harness, model catalog/profiles and QEMU scripts.
- `docs/requirements/`: market, master, and software requirements.

## Change workflow
1. Read `conversation.md`, `AGENTS.md`, `llms.md`, architecture, relevant profile and SRS.
2. Identify whether a change affects wire compatibility, HAL ABI, platform binding, device profile, or safety behavior.
3. Update requirements and tests before implementation when a behavior or contract changes.
4. Keep OS headers/APIs out of `Shared/`; route them through a platform or transport adapter.
5. Run `make -C Shared clean test`, relevant Python tests, Linux user client, and Windows WDK build when available.
6. Include a verification table in change notes listing command, target, result and limitations.
7. Update profile/platform status and training material when behavior or prerequisites change.

## Build commands
```bash
make -C Shared clean test
make -C Linux user-client
python -m unittest discover -s DriverTest/tests -v
```
Windows project: build `UMDF 2 Driver1/UMDF 2 Driver1.vcxproj` as Debug x64 with the WDK. Linux kernel module: use only exact matching kernel headers/config. Android, macOS, and FreeRTOS require their native SDK/BSPs.

## API and ABI policy
- Use explicit endian encoding and fixed-width wire fields; do not serialize native struct layout.
- Version incompatible protocol changes and maintain golden vectors.
- Document buffer ownership, output capacity, timeout, cancellation, error mapping, and unsupported capabilities.
- Avoid generic raw register/MMIO operations in user-accessible APIs.

## Code review checklist
- Bounds and integer arithmetic checked before pointer use.
- User pointers copied and validated using the target OS's documented API.
- Every accepted async request has one completion and defined cancel/remove behavior.
- Access control is least privilege; logs do not reveal secrets.
- Tests include negative and failure paths, not only happy paths.
- GPL/provenance and third-party dependency compatibility are checked.
- No test result or simulator is described beyond its actual coverage.
