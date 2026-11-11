# Static site and CI workflow

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

`.github/workflows/factory-ci.yml` runs Python schema/tests, strict C11 host tests, deterministic dossier package generation, an unsigned SHA-256 inventory of generated artifacts, and a dependency-light HTML export. Hash inventories detect accidental/tampered changes but are not signatures, trusted timestamps, or identity attestation. On pushes to `master`, it deploys `docs/_site` to GitHub Pages; configure Pages to use GitHub Actions in repository settings. Pull requests produce downloadable site/evidence draft artifacts but do not deploy.

Six Jupyter-format notebook documents in `notebooks/` are executed cell-by-cell by `python -m driver_factory.site`; they use only the Python standard library. The exporter is intentionally small, not a full CommonMark/Jupyter renderer; it emits executable lesson content as static source/code displays. Test execution occurs at build time. Pages clearly state that the content is educational and not product validation or certification.

The workflow reports optional tool availability; it does not mark missing CBMC, Frama-C, QEMU, Renode, Windows/macOS/QNX/RTOS SDKs, or HIL as passed. Add dedicated runners only after their licenses, provisioning, versions, safety, and reproducibility are controlled. Artifact contents must not include proprietary OS images, keys, private device traces, patient data, or production secrets.
