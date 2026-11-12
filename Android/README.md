# Android HAL binding starter

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

This AIDL NDK service exposes the shared **reference/test** PING/ECHO protocol to Android clients and calls the same C core. It is not a hardware driver and does not replace the Linux kernel bus/function driver. A product binding must connect the service to its device node/vendor interface and enforce SELinux, permission, and lifecycle policy.

The repository-level `Android.bp` is a development Soong starter. It has not been built here because Android NDK/AOSP/Soong is not installed. Before product use, choose the Android release and vendor partition, convert the unstable AIDL interface to a versioned/frozen VINTF-stable interface if it crosses system/vendor boundaries, add service/init/VINTF manifests, SELinux policy, and hardware-specific error/cancellation behavior.

The Android framework does not provide a universal kernel driver HAL for USB, Bluetooth, Wi-Fi, PCI, storage, robot, and accelerator devices. Those require the corresponding Linux kernel driver and vendor-specific Android HAL/framework integration.
