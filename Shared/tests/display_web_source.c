/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#define _POSIX_C_SOURCE 199309L
#include "display_sim.h"
#include "display_transport.h"
#include <stdio.h>
#include <time.h>

static void render(dpc_display *display, dpc_u32 frame)
{
	dpc_display_color background = {12U, 18U, 32U, 255U};
	dpc_display_color accent = {30U, 180U, 255U, 255U};
	dpc_display_rect moving = {(frame * 4U) % 540U, 180U, 100U, 100U};
	dpc_u64 fence = 0U;
	(void)dpc_display_clear(display, background);
	(void)dpc_display_fill_rect(display, moving, accent);
	(void)dpc_display_present(display, &fence);
	(void)dpc_display_wait_fence(display, fence);
}

int main(void)
{
	dpc_display *display = dpc_display_create();
	dpc_display_mode mode = {640U, 480U, 60U};
	dpc_u32 frame;
	if (display == NULL || dpc_display_start(display) != DPC_HAL_ACCEPTED || dpc_display_set_mode(display, mode) != DPC_HAL_ACCEPTED) return 1;
	for (frame = 0U; frame < 600U; ++frame) {
		const dpc_u8 *pixels;
		dpc_size_t length;
		dpc_display_transport_header header = {DPC_DISPLAY_TRANSPORT_MAGIC, DPC_DISPLAY_TRANSPORT_VERSION, mode.width, mode.height, 0U, frame};
		render(display, frame);
		if (dpc_display_framebuffer(display, &pixels, &length) != DPC_HAL_ACCEPTED) return 2;
		header.payload_length = (dpc_u32)length;
		if (fwrite(&header, sizeof(header), 1U, stdout) != 1U || fwrite(pixels, length, 1U, stdout) != 1U) return 3;
		fflush(stdout);
		{ struct timespec delay = {0, 16000000L}; nanosleep(&delay, NULL); }
	}
	dpc_display_destroy(display);
	return 0;
}
