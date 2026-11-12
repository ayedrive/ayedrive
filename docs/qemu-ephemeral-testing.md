# Low-storage QEMU guest workflow

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## Policy
The runner does not download operating-system media, bypass licensing, or retain virtual disks by default. Supply an installer/live ISO that you are authorized to use and its SHA-256 from an independent trusted source. The runner creates one temporary qcow2 guest, attaches only the ISO and that virtual disk, disables networking/shared folders/host block passthrough, and removes the temporary disk when QEMU exits. Set `KEEP_GUEST_WORKDIR=1` only if you explicitly need to preserve the guest. The ISO itself is user-provided and not copied into the work directory.

A requested disk size is a sparse qcow2 virtual maximum; actual consumption grows while the guest writes and may exceed estimates. `MIN_FREE_MB` provides a floor/reserve check, not a guarantee of available disk. Choose `WORK_ROOT` on the intended filesystem. Do not run while C: is critically low; use a roomy scratch volume. Do not give the guest host physical disks or secrets.

## Run

```bash
GUEST_ISO="$HOME/authorized-linux.iso" \
GUEST_ISO_SHA256="<trusted-64-hex-digest>" \
WORK_ROOT="$HOME/driver-factory-temp" \
GUEST_RAM_MB=2048 GUEST_DISK_GB=8 MIN_FREE_MB=4096 \
bash DriverTest/qemu/run-ephemeral-guest.sh
```

The script starts an interactive QEMU guest; it does not install a driver or claim guest validation. To make an OS/adapter validation repeatable, provision an approved guest in a separate automation job, install the exact package, run the target test harness, collect logs/configuration hashes, shut down, and discard the guest disk. A successful boot is not a driver, physical-device, HIL, or safety result.

## Image-source automation boundary
Automated download is deliberately not enabled by default. OS media has different license/redistribution terms, vendor redirects, checksum-signature formats, sizes, and security updates. Add a per-OS downloader only with a documented official source, user/runner license acceptance, independently authenticated checksum/signature, maximum size, and cleanup policy. Do not store proprietary ISOs in GitHub artifacts or Pages.

## Current local evidence
WSL has QEMU 8.2.2 and qemu-img. The pre-existing Ubuntu 26.04 installer ISO in Downloads was SHA-256 checked against the publisher's `SHA256SUMS` and launched headlessly using this runner with networking and passthrough disabled. QEMU ran for a bounded 120 seconds; the temporary qcow2 was deleted as expected. A follow-up serial-capture attempt produced no guest output, so this is launch/cleanup validation only—not confirmed OS boot, driver installation, or adapter integration. Set `QEMU_SERIAL_LOG` to a host path to capture a guest serial console on future runs. No image was downloaded. Do not retain guest artifacts or rerun without considering the available space on the selected work filesystem.
