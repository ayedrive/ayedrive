// SPDX-License-Identifier: GPL-3.0-only
// Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
package com.example.dpc;

interface IDpcProtocol {
	byte[] exchange(in byte[] request);
}
