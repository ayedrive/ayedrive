# Linux reference binding

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

`kernel/dpc_reference.c` is a root-only (`0600`) misc-device software test binding for the reference PING/ECHO protocol. It is not a hardware driver and does not emulate a USB/PCI/physical device. The native Linux device class/bus binding for each physical model remains separate. It compiles the same portable protocol source into the module.

Build the user-mode test client with GCC:

```bash
cd Linux
make user-client
```

Build the module only on a Linux system with headers matching the running kernel:

```bash
make kernel KDIR=/path/to/matching/kernel/build
```

After an authorized test VM loads `kernel/dpc_reference.ko`, verify the restricted device node and run the test client:

```bash
sudo insmod kernel/dpc_reference.ko
sudo ./build/test_protocol
sudo rmmod dpc_reference
```

The current WSL kernel has no matching headers installed, so a loadable module build/load is not verified here. Build and test the module in a disposable Linux VM whose kernel configuration and headers match its running kernel. Do not load an unreviewed kernel module on a production system.
