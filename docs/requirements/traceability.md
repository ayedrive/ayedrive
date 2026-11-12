# Requirements traceability

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

This is the initial traceability baseline. Evidence marked **pending** must not be inferred from a profile or source stub.

| Requirement | Implementation / document | Verification evidence | State |
|---|---|---|---|
| MR-001 / SRS-CORE-001 | `Shared/core/device_protocol.c` | malformed/bounds unit tests | Implemented; host tests pass |
| MR-002 / SRS-CORE-002–004 | `Shared/include/device_protocol.h`, core tests | C and C++ host test runs | Implemented; host tests pass |
| MR-003 / SRS-HAL-001–003 | `Shared/include/device_hal.h` | ABI review and per-binding contract tests | Interface defined; async binding tests pending |
| MR-004 / SRS-PLAT-001–002 | `UMDF 2 Driver1/`, `Linux/`, `Android/`, `MacOS/`, `FreeRTOS/` | native build and lifecycle test per platform | Windows built; Linux client built; other native tests pending |
| MR-005–006 | `DriverTest/models/profiles/`, `docs/device-modeling.md` | accepted device dossier and model review | Templates only; physical specs pending |
| MR-007 | platform bindings and `docs/test-plan.md` | negative tests, static analysis, race/fault injection | Partial; per-platform evidence pending |
| MR-008 | `DriverTest/`, platform test plans | isolated VM/simulator test logs | Windows host harness exists; live guest run pending |
| MR-009 / SRS-SEC-* | `docs/critical-systems-validation.md` | threat analysis and access-control tests | Guidance only; product-specific evidence pending |
| MR-010 | `docs/platform-matrix.md`, `docs/roadmap.md` | build logs with toolchain/target identity | Partial; unavailable native SDKs documented |
| MR-011–012 | `LICENSE`, `NOTICE.md`, `docs/license-and-provenance.md` | full source/dependency provenance audit | Repository text audit performed; provenance audit pending |
| MR-013 | `MRD-master.md`, `SRS.md`, this table | reviewer confirms links and status | Initial baseline |
| MR-014 / SRS-SAFE-001–007 | `docs/critical-systems-validation.md`, `docs/safety/` | product-specific standards approval, hazard/threat analysis, safety case, independent review and target evidence | Framework and preliminary reference assessment exist; no product approved; no safety claim |
| MR-015 | `.gitignore`, local `.git` | `git remote -v` empty; commit record | Local repository and initial commit exist; no remote |
| MR-016 | `docs/training/` | learner assessment records | Curriculum drafted; delivery/assessment pending |
| MR-017 | `docs/safety/deployment-profile.schema.json`, deployment profile | schema validation and authority-approved profile | Schema and non-safety sample profile exist; product profile pending |
| MR-018 | `docs/safety/README.md`, product architecture and tests | mode/security design review and target fail-closed/control safety tests | Policy specified only; runtime control gating not implemented |
| MR-019 | `docs/architecture.md`, `docs/safety/` | product safety-case review of reused components | Boundary documented; deployment reviews pending |
