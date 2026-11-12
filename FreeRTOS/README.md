# FreeRTOS binding starter

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

`src/dpc_freertos_binding.c` is a static-allocation task/queue binding for the reference PING/ECHO protocol. It uses FreeRTOS queue and task APIs, then calls the same portable C core. It does not initialize a bus or hardware device; board-specific USB, Bluetooth, Wi-Fi, PCI, storage, robot, and accelerator APIs must be supplied by the selected BSP/vendor stack.

The caller retains request and response buffers until the completion callback executes. Start the service during board initialization before submissions; this starter has no stop/cancel path and is not suitable for production lifecycle management without a target-specific design. The selected FreeRTOS configuration must enable static task/queue allocation and provide `configMINIMAL_STACK_SIZE`.

No FreeRTOS kernel or board toolchain is installed in this workspace, so this binding is not compiled or hardware-validated here. Record the FreeRTOS release, board, compiler, interrupt priorities, DMA/cache policy, and memory budget before adopting it.
