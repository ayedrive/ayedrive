# AyeDrive agent instructions

## Project

AyeDrive is an automated end-to-end device driver ecosystem by AyeAI. The
repository currently contains only the project description and license; the
target platforms, implementation, and tooling have not yet been established
here. Inspect the repository and the user's request before choosing an
architecture or making assumptions about existing capabilities.

## Working in this repository

- Make focused changes that advance the requested work and preserve existing
  functionality. Follow established project conventions as they emerge.
- Choose appropriate platform APIs and tools for the task; explain any
  significant assumptions when requirements are unspecified.
- Use existing dependencies and project tooling where possible. Document any
  new prerequisites, setup steps, and commands alongside the affected code.
- Add focused tests for new behavior when test infrastructure exists or is
  introduced. Run relevant checks and report what was verified and what could
  not be verified. Do not invent build or test commands.
- Respect the repository's GPL-3.0 license and do not commit secrets, generated
  build outputs, or vendor code without a clear need.

## Device driver safety

- Treat device access, kernel code, driver installation, signing, firmware
  updates, and elevated privileges as security-sensitive and platform-specific.
- Validate inputs, minimize privileges, and handle errors and resource cleanup.
  Do not disable security protections or bypass driver signing.
- Do not install drivers, modify firmware, access physical devices, or make
  destructive system changes without explicit approval. Prefer mocks or
  isolated test environments for automated verification.
- Never claim hardware or platform support, driver signing, or end-to-end
  functionality that has not been implemented and verified.
