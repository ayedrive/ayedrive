# Portable protocol core

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

This C99 library and test suite are OS-independent. It defines a small **reference/test protocol** (PING and ECHO) so every platform binding can demonstrate the same bytes and response semantics. This is not a protocol for the user's physical devices; replace or extend the protocol module only from a device specification.

The `device_hal.h` header defines the portable HAL request/lifecycle contract. Platform bindings supply native bus behavior and must document how native cancellation, buffer lifetime, reset, clocks, and device capabilities map to it. The core itself makes no OS calls and allocates no memory.

Run the C and C++ linkage host tests in WSL or another C99/C++17 environment:

```bash
cd Shared
make test
```
