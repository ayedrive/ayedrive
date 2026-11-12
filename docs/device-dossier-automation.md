# Device dossier intake and automation workflow

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Goal
Collect, validate, and trace device facts without inventing undocumented behavior. The factory can gather machine-readable host/device observations and organize supplied manuals, traces, and test evidence. It cannot infer a vendor protocol or replace permission, device-owner approval, or physical inspection.

## Inputs and outputs
- Start from `DriverTest/models/device-dossier.template.json`.
- Validate against `device-dossier.schema.json`; keep status at `intake` or `awaiting_evidence` while any field is unknown.
- Record document title/revision/page, URL or local provenance, retrieval date, SHA-256, license/redistribution terms, and reviewer for each supplied artifact.
- Capture OS-reported identifiers and interface topology using platform-native read-only inventory methods; do not read/write arbitrary MMIO or firmware.
- For USB, collect descriptor trees; PCI/PCIe, IDs/configuration/BAR sizes only with safe permissions; Bluetooth, controller/HCI/version and advertised profiles; Wi-Fi, adapter/firmware and supported interface capabilities; storage, geometry/identify data read-only; network/fieldbus, protocol captures supplied by owner; stream/motion devices, timestamped owner-provided traces and calibration metadata.
- Generate a missing-evidence report, device-model capability matrix, candidate tests, and traceability links. A human device owner reviews before `accepted_for_modeling`.

## Trust and safety
1. Inventory is read-only by default and requires explicit user selection of a device; avoid scanning unknown buses by default.
2. Do not query privileged configuration/register spaces or perform firmware/update/reset, raw I/O, destructive writes, or bus reconfiguration during dossier collection.
3. Never collect serial numbers, patient data, keys, network secrets, or proprietary payloads unless required and authorized; redact before committing.
4. Store raw captures outside Git unless redistribution is authorized; commit hashes and provenance references instead of large/private binaries.
5. A driver or model may not claim physical compatibility until the dossier is complete, provenance reviewed, golden vectors approved, and model fidelity checked against the exact device/firmware.
6. Safety-relevant dossiers link to a deployment profile, but intake does not select an integrity class or grant approval.

## Automation slices
- **Current:** JSON dossier structural validation; deterministic package with normalized input, missing-evidence report, architecture diagram source, per-interface HAZOP prompts, and requirements/source trace links. Evidence indexing hashes supplied files and leaves safety approval and independent review open.
- **Next:** read-only inventory collector plugins with user consent and tests on disposable targets. Collector output is an observation, not an accepted specification. JSON Schema validation is run by `tools/driver_factory/validate_schemas.py` in hosted and local CI; the dependency is pinned in `tools/requirements-ci.txt`.
- **Later per device:** approved golden trace import, simulator conformance tests, error/fault injection plan and HIL evidence links.

## Review transitions
`intake` → `awaiting_evidence` → `review_ready` → `accepted_for_modeling` or `rejected`. Only a named device owner/reviewer may approve identity/protocol facts; a separate safety authority assesses safety-domain claims. No automated transition grants certification or control authorization.

## Reproducible local checks
Run `powershell.exe -NoProfile -ExecutionPolicy Bypass -File .\scripts\ci-local.ps1` from the repository root to execute the portable checks mirrored by CI. The script does not change the machine execution policy. It does not deploy GitHub Pages or run native SDK, QEMU guest, physical HIL, or product-approval checks; those remain separate gates.
