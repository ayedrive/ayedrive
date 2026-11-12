/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */

/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    driver and application

--*/

#include <winioctl.h>

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_UMDF2Driver1,
    0x1495036f,0xaa8f,0x459f,0xbf,0x25,0xf2,0xcf,0xdf,0x85,0x45,0x2c);
// {1495036f-aa8f-459f-bf25-f2cfdf85452c}

#define IOCTL_DPC_PROTOCOL_MESSAGE \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
