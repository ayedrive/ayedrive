# License and provenance policy

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

Original repository material is designated GPL-3.0-only. The copyright holder retains ownership and may separately license owned contributions; recipients retain the rights granted by GPLv3. This statement does not grant rights to third-party material.

Before importing code, generated templates, SDK snippets, firmware, protocol specifications, or test fixtures:
1. Record source, version/commit, author/copyright holder, license, modifications, and redistribution obligations.
2. Confirm license compatibility with GPL-3.0-only and obligations for generated/binary artifacts.
3. Keep upstream notices intact; never replace another author's copyright with this repository notice.
4. Exclude vendor SDK headers/libraries and proprietary firmware from source/package media unless redistribution is expressly permitted.
5. Review the Visual Studio/WDK-generated sample and each template's provenance before representing every line as solely owned original code.
6. Record third-party dependencies and license evidence in `docs/provenance/` before release.

## Strict QEMU source policy

QEMU is an external build and validation dependency, not repository content. AyeDrive must never carry, vendor, modify, or distribute QEMU source code. QEMU use-case build scripts and Makefiles may obtain source only from the host package source mechanism or the official QEMU upstream repository; they must pin and verify the QEMU version and keep the checkout and build output outside the repository. Release archives contain only AyeDrive-owned integration contracts, scripts, tests, and provenance records.

The presence of `LICENSE` and SPDX labels is not a legal determination that all existing or generated content is owned by the named copyright holder. Resolve provenance gaps before public redistribution or relicensing.
