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

## Current host limitation
QEMU and qemu-img were not found on the inspected Windows/WSL host. C: had approximately 39 GB free and was 89% used. Therefore only the script’s syntax and safety/parameter validation can be checked here; no guest image was downloaded, no QEMU process was run, and no VM integration result is claimed.
