# Copilot Instructions

## Project Guidelines
- Use Git author identity Abhishek Choudhary <ayeai.ayeam@gmail.com> for this repository.
- Strict QEMU policy: AyeDrive never carries, vendors, or distributes QEMU source/code. For QEMU device-use scenarios, project build scripts/Makefiles may obtain the official QEMU source (or host package source) at build time, pin/validate the QEMU version, build against it, and run validation. QEMU remains an external build dependency; only AyeDrive adapters/contracts and scripts belong in the repository.

## Testing Guidelines
- QEMU guest testing with licensed media/checksum, native adapter SDK builds, and simulator/collector work are locally testable in this setup; do not assume these are unavailable without checking current local tools and supplied inputs.
- When building/testing OS targets, do not stop at missing local SDKs or images: inspect availability, acquire official/licensed toolchains and media when authorized, create deterministic virtual device models from the project contracts when no product dossier exists, and test them locally. Clearly distinguish simulated evidence from physical HIL or certification.
- Use QEMU device simulations as-is. Build an AyeDrive-side adapter layer that consumes QEMU's existing public interfaces and maps them into the portable HAL; do not patch, vendor, or carry QEMU source, and do not build a guest in-OS HAL path. This adapter pattern should generalize across QEMU device families and remain portable across host OSes.

## Workflow Guidelines
- Keep scope separated into explicit stages instead of conflating device implementation, visual demos, acceleration backends, signing/security remediation, and broader device rollout.
- For Windows display artifacts, use a self-signed development certificate and update the overall build/sign/verify/run process.- For Windows display artifacts, use a self-signed development certificate and update the overall build/sign/verify/run process.