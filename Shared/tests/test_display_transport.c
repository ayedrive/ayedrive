/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "display_transport.h"

extern int dpc_display_transport_valid(const dpc_display_transport_header *header);

int main(void)
{
	dpc_display_transport_header header = {DPC_DISPLAY_TRANSPORT_MAGIC, DPC_DISPLAY_TRANSPORT_VERSION, 2U, 2U, 16U, 1U};
	return dpc_display_transport_valid(&header) ? 0 : 1;
}
