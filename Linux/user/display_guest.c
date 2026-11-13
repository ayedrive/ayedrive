/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#define _POSIX_C_SOURCE 199309L
#include "display_sim.h"
#include "display_transport.h"

#include <errno.h>
#include <fcntl.h>
#ifndef O_CLOEXEC
#define O_CLOEXEC 0
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int write_all(int descriptor, const void *buffer, size_t length)
{
	const unsigned char *bytes = buffer;
	while (length != 0U) {
		ssize_t written = write(descriptor, bytes, length);
		if (written <= 0) {
			if (written < 0 && errno == EINTR) continue;
			return -1;
		}
		bytes += (size_t)written;
		length -= (size_t)written;
	}
	return 0;
}

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

int main(int argc, char **argv)
{
	const char *port_path = argc > 1 ? argv[1] : "/dev/virtio-ports/ayeadrive.display";
	dpc_display_mode mode = {640U, 480U, 60U};
	dpc_display *display = dpc_display_create();
	const dpc_u8 *pixels;
	dpc_size_t length;
	int port;
	dpc_u32 frame;
	(void)fprintf(stderr, "DPC_GUEST_DISPLAY_START\n");
	if (display == NULL || dpc_display_start(display) != DPC_HAL_ACCEPTED || dpc_display_set_mode(display, mode) != DPC_HAL_ACCEPTED) return 1;
	(void)fprintf(stderr, "DPC_GUEST_HAL_READY\n");
	port = open(port_path, O_WRONLY | O_CLOEXEC);
	if (port < 0) {
		(void)fprintf(stderr, "open %s: %s\n", port_path, strerror(errno));
		dpc_display_destroy(display);
		return 2;
	}
	for (frame = 0U; frame < 600U; ++frame) {
		dpc_display_transport_header header = {DPC_DISPLAY_TRANSPORT_MAGIC, DPC_DISPLAY_TRANSPORT_VERSION, mode.width, mode.height, 0U, frame};
		render(display, frame);
		if (dpc_display_framebuffer(display, &pixels, &length) != DPC_HAL_ACCEPTED || length > DPC_DISPLAY_TRANSPORT_MAX_FRAME) break;
		if (frame == 0U) (void)fprintf(stderr, "DPC_GUEST_FIRST_FRAME length=%zu\n", (size_t)length);
		header.payload_length = (dpc_u32)length;
		if (write_all(port, &header, sizeof(header)) != 0 || write_all(port, pixels, length) != 0) { (void)fprintf(stderr, "DPC_GUEST_WRITE_FAILED errno=%d\n", errno); break; }
		struct timespec delay = {0, 16000000L};
		nanosleep(&delay, NULL);
	}
	(void)close(port);
	dpc_display_destroy(display);
	return frame == 600U ? 0 : 3;
}
