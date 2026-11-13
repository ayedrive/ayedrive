# Shared core and platform binding architecture

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

## AyeDrive simulation architecture

AyeDrive adds a separate behavioral simulation plane under `tools/ayeadrive_sim/`. Each requested device family is realized as an independently launched `AyeAI: AyeAM <device> sim` process with a JSON-lines boundary. `tools/ayeadrive_sim/harness.py` is implementation-independent: it drives the process protocol, checks family-specific exercises, lifecycle, faults, and boundary responses, and emits audit JSON/Markdown. This plane validates executable behavioral contracts; it does not replace platform bindings or physical-device evidence.

## Goal
Keep device protocol/state-machine logic in one portable C core where the protocol is genuinely identical. Each operating system gets its own driver/service binding. A binding is not a thin alias: it translates that OS's device model, bus requests, memory ownership, cancellation, power, and security rules into the shared core contract.

## Layers

1. **Device profile** — hardware identity, bus topology, protocol version, endpoints/registers/queues, capabilities, and test fixtures. Each physical model is explicit; a profile cannot infer arbitrary hardware behavior.
2. **Platform binding** — Windows UMDF/KMDF, Linux kernel or userspace driver boundary, Android kernel/HAL service boundary, macOS DriverKit, or RTOS task/ISR binding. Owns OS objects, I/O request lifetime, synchronization, buffer mapping, interrupt/DMA lifecycle, power transitions, and permissions.
3. **Portable protocol core** — C code with no OS headers or system calls. Validates versioned protocol messages, bounds, state transitions, and protocol responses. It talks to a HAL contract rather than a platform API.
4. **HAL implementation** — translates core operations into a concrete bus/controller/device operation. HAL implementations are transport- and platform-specific; a USB transfer HAL is not interchangeable with PCI MMIO or block I/O.
5. **Test model** — deterministic fake HAL for host unit tests, then a simulator or physical device for integration and hardware-in-the-loop tests.

## Proposed C contract

The shared ABI uses fixed-width fields and explicit byte serialization. Do not send compiler-packed C structs across an OS or hardware boundary. Messages carry a protocol version, operation, payload length, and request identifier; every parser checks minimum/maximum lengths, integer overflow, allowed operations, and response capacity before accessing data.

The core consumes an opaque context and a versioned set of HAL operations. Required lifecycle hooks are start, submit/transfer, cancel, reset, stop, and monotonic time; optional capabilities are advertised explicitly. Every asynchronous request has a single owner, a completion result, cancellation semantics, and a documented buffer lifetime. Each binding maps native status/error values at the boundary.

The HAL is deliberately capability-based: device-specific code can request control/bulk/interrupt transfers, register/window access, queue submission, block operations, or network packet operations only when the selected profile and binding support them. The core must return NOT_SUPPORTED rather than silently emulate a missing capability.

## Driver families and platform bindings

- **Windows:** UMDF/KMDF are distinct bindings. NDIS, Storport, HID, USB, Bluetooth, Wi-Fi, PCI and other class/function drivers need their respective Windows contracts; they are not implemented by modifying one generic UMDF queue callback.
- **Linux:** kernel bus/character/network/block drivers use kernel APIs and Kbuild. User-space HALs (for example, libusb or device nodes) are separate and do not replace a kernel driver.
- **Android:** kernel drivers remain Linux drivers; Android-specific framework integration belongs in a HAL/service layer (AIDL/HIDL as appropriate to the target release). Binder interfaces do not replace kernel bus drivers.
- **macOS:** new device drivers use DriverKit/system extensions where supported. DriverKit availability, entitlements, matching rules, and supported device classes must be checked for the target macOS release; legacy kernel extensions are not a general new-driver path.
- **FreeRTOS:** a port supplies task/queue/critical-section/timer and bus callbacks. The RTOS itself does not define one universal driver model; board support and vendor peripheral APIs remain explicit.
- **Other targets:** Zephyr, QNX, VxWorks, FreeBSD, embedded Linux, and hypervisor-specific models need separate bindings and SDK review. Add them as independent ports, not OS conditionals spread through the shared core.

## Cross-platform invariants

- The protocol version and wire encoding are identical across ports when the device protocol is identical.
- OS-native device identifiers, interface paths, permissions, power policies, DMA mappings, and interrupt handling remain platform-specific.
- A virtual device must implement the relevant bus and device protocol. A generic QEMU NIC, USB controller, or PCI bus is not a substitute for an arbitrary physical device.
- Host tests validate the core with fake HALs; VM integration validates a specific binding/model; hardware-in-the-loop validates a particular physical device and firmware revision.
- Destructive storage/control tests require isolated throwaway media and explicit opt-in.

## Emulator and QEMU integration matrix

QEMU is an optional execution environment, not a required layer for native host-OS or physical-device paths. The project supports two device providers:

- **AyeDrive device simulation** — the preferred provider, implemented against the portable HAL.
- **External emulator device simulation** — a compatibility provider, accessed through an AyeDrive compatibility shim when a direct AyeDrive simulation is not yet available.

The compatibility shim is an adapter owned by AyeDrive. It does not make QEMU, Renode, or another emulator part of the portable core, and it does not change the existing OS adapters.

### Native host-OS paths

There is no QEMU in the preferred native path:

```text
Host OS <-> AyeDrive HAL <-> AyeDrive device simulation
```

The short-term compatibility path may use an existing QEMU device model without making QEMU a native OS requirement:

```text
Host OS <-> AyeDrive HAL <-> AyeDrive compatibility shim <-> QEMU device simulation
```

### QEMU paths

When the device is exposed through QEMU, QEMU is the outer execution environment:

```text
Host OS <-> QEMU <-> AyeDrive HAL <-> AyeDrive device simulation
```

The quick compatibility form is:

```text
Host OS <-> QEMU <-> AyeDrive HAL <-> AyeDrive compatibility shim <-> QEMU device simulation
```

The QEMU adapter/shim is distinct from Windows, Linux, Android, macOS, and RTOS/Arduino OS adapters.

### Guest OS running on QEMU

The host-side QEMU device path and the guest-side OS path are independent layers. Preferred combinations use AyeDrive device simulations on both sides:

```text
Host OS <-> QEMU [QEMU <-> AyeDrive HAL <-> AyeDrive device simulation]
		<-> Guest OS <-> AyeDrive HAL <-> AyeDrive device simulation
```

Each side may temporarily use the compatibility shim instead:

```text
Host OS <-> QEMU [QEMU <-> AyeDrive HAL <-> compatibility shim <-> QEMU device simulation]
		<-> Guest OS <-> AyeDrive HAL <-> compatibility shim <-> QEMU device simulation
```

Mixed combinations are also valid: either the host-side or guest-side layer may use the preferred simulation while the other uses the compatibility shim. The host and guest HAL instances remain separate and must not be conflated.

### Unmodified QEMU

Unmodified QEMU remains supported, with limitations such as boot-time display/video behavior:

```text
Host OS <-> QEMU [unmodified QEMU <-> QEMU device simulation]
		<-> Guest OS <-> AyeDrive HAL <-> AyeDrive device simulation
```

The quick guest compatibility form is:

```text
Host OS <-> QEMU [unmodified QEMU <-> QEMU device simulation]
		<-> Guest OS <-> AyeDrive HAL <-> compatibility shim <-> QEMU device simulation
```

The same matrix applies to another applicable open-source emulator. QEMU source is an external dependency; it is not part of the portable AyeDrive core.

### Strict QEMU source policy

AyeDrive does not carry, vendor, modify, or distribute QEMU source code. For QEMU device-use scenarios, native-host build scripts and Makefiles may obtain QEMU only from the host package source mechanism or the official QEMU upstream repository, pin and verify the intended QEMU version, build against that external checkout, and run validation. The checkout and build output remain external artifacts and must not be committed to this repository. Only AyeDrive-owned adapters, contracts, scripts, tests, and provenance records belong here. The same policy applies to another open-source emulator used as an external provider.

## Critical-system boundary
This repository is a development/training scaffold, not a safety certification or production-readiness claim. Before use in a safety-related system, establish the applicable standard and safety case, perform hazard/risk analysis, requirements traceability, threat analysis, independent design/code review, static analysis, boundary/fault injection, concurrency and power-failure tests, hardware-in-the-loop testing, release controls, and audit evidence. The required standard depends on the domain (for example, automotive, medical, industrial, aerospace, or rail); no SIL/ASIL or certification level is assumed here.

## Product-specific safety configuration
Safety governance is a product/deployment concern, not a protocol operation in the shared HAL. See `docs/safety/` for the deployment-profile schema, candidate domain profiles, assessment templates, and preliminary reference assessment. Profiles select candidate review scopes and link controlled evidence; they do not activate safety functions or prove compliance. The product default is monitor-only. A monitoring role still requires product-specific freshness, quality, fault, alarm, and operator-response requirements. Any control function requires its own verified product architecture, explicit authority, safe-state behavior, independent protection, target evidence, and approval. Reuse of core/HAL source does not transfer assurance or certification between deployments.

## Virtual model boundary
`Shared/core/device_store.c` is a deterministic in-memory transactional-store reference with explicit fault cut points. It does not yet implement the common bus/DMA/IRQ/VFS HAL, a QEMU device, or physical NVRAM. Its commit-marker atomicity is a model assumption only. See `docs/simulators/reference-nvram.md` for exact guarantees and limits. Keep simulator models behind an explicit backend contract and require per-device conformance/fidelity evidence before making sim-to-real claims.
