# Copilot Instructions

## Project Guidelines
- Use Git author identity Abhishek Choudhary <ayeai.ayeam@gmail.com> for this repository.

## Testing Guidelines
- QEMU guest testing with licensed media/checksum, native adapter SDK builds, and simulator/collector work are locally testable in this setup; do not assume these are unavailable without checking current local tools and supplied inputs.
- When building/testing OS targets, do not stop at missing local SDKs or images: inspect availability, acquire official/licensed toolchains and media when authorized, create deterministic virtual device models from the project contracts when no product dossier exists, and test them locally. Clearly distinguish simulated evidence from physical HIL or certification.