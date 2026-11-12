# Earth ↔ Mars training notebooks

SPDX-License-Identifier: GPL-3.0-only
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

Seven executable Jupyter-format lessons are exported as static HTML by `python -m driver_factory.site`:
1. Interface impedance and four-layer boundaries.
2. Earth to Mars: illustrative timing budgets (not WCET measurement).
3. Mars to Earth: requirement traceability and evidence status.
4. OS Rosetta Stone: responsibility boundaries and target-specific adapter limits.
5. POSIX/VFS and transactional NVRAM analogy (not durable storage).
6. Formal/fault-injection checklist (all gates remain open unless independently executed).
7. AyeDrive synthetic device models and evidence boundaries.
8. AyeDrive maximum-coverage independent simulation, audit artifacts, and fail-fast release mode.

CI executes code cells with the Python standard library. Notebooks contain no privileged hardware access, downloads, or claims of certification. Static export displays source and explanatory outputs; it does not embed a live backend.
