# macOS DriverKit binding starter

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

`DriverKit/dpc_protocol_bridge.c` exposes the shared reference protocol core to a target-specific DriverKit external-method implementation. This bridge alone is **not** a loadable macOS driver: a native DriverKit service, user client, matching rules, project, signing, entitlements, and host app must be created for the selected supported device family.

This workspace runs on Windows/WSL and has no macOS SDK or Xcode. The DriverKit bridge has not been built. Complete and validate the native extension on a supported macOS/Xcode host, following the target release's DriverKit API and entitlement requirements. Do not copy legacy kernel-extension code or assume that a hardware class is supported by DriverKit; confirm support for the exact bus/device and macOS version first.
