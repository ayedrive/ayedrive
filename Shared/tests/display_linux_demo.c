/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#define _POSIX_C_SOURCE 199309L

#include "display_sim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(DPC_DISPLAY_X11)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

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

#if defined(DPC_DISPLAY_X11)
static int run_visible(dpc_display *display, dpc_display_mode mode)
{
	Display *xdisplay = XOpenDisplay(NULL);
	Window window;
	GC gc;
	XImage *image;
	const dpc_u8 *pixels;
	dpc_size_t length;
	dpc_u32 frame = 0U;
	if (xdisplay == NULL) {
		(void)fprintf(stderr, "DISPLAY is unavailable; use --headless or start WSLg/X11.\n");
		return 2;
	}
	window = XCreateSimpleWindow(xdisplay, DefaultRootWindow(xdisplay), 0, 0, mode.width, mode.height, 1, 0, 0);
	XStoreName(xdisplay, window, "AyeDrive C Display Simulator");
	XSelectInput(xdisplay, window, ExposureMask | KeyPressMask | StructureNotifyMask);
	XMapWindow(xdisplay, window);
	gc = XCreateGC(xdisplay, window, 0, NULL);
	image = XCreateImage(xdisplay, DefaultVisual(xdisplay, 0), DefaultDepth(xdisplay, 0), ZPixmap, 0, NULL, mode.width, mode.height, 32, 0);
	if (image == NULL || image->bits_per_pixel != 32) return 3;
	for (;;) {
		XEvent event;
		while (XPending(xdisplay) != 0) {
			XNextEvent(xdisplay, &event);
			if (event.type == DestroyNotify || event.type == KeyPress) goto done;
		}
		render(display, frame++);
		if (dpc_display_framebuffer(display, &pixels, &length) != DPC_HAL_ACCEPTED || length < (dpc_size_t)mode.width * mode.height * 4U) break;
		image->data = (char *)pixels;
		XPutImage(xdisplay, window, gc, image, 0, 0, 0, 0, mode.width, mode.height);
		XFlush(xdisplay);
		image->data = NULL;
		struct timespec delay = {0, 16000000L};
		nanosleep(&delay, NULL);
	}
done:
	image->data = NULL;
	XDestroyImage(image);
	XFreeGC(xdisplay, gc);
	XDestroyWindow(xdisplay, window);
	XCloseDisplay(xdisplay);
	return 0;
}
#endif

int main(int argc, char **argv)
{
	const int headless = argc > 1 && strcmp(argv[1], "--headless") == 0;
	dpc_display *display = dpc_display_create();
	dpc_display_mode mode = {640U, 480U, 60U};
	dpc_u32 frame;
	if (display == NULL || dpc_display_start(display) != DPC_HAL_ACCEPTED || dpc_display_set_mode(display, mode) != DPC_HAL_ACCEPTED) return 1;
#if defined(DPC_DISPLAY_X11)
	if (!headless) {
		int result = run_visible(display, mode);
		dpc_display_destroy(display);
		return result;
	}
#else
	if (!headless) {
		(void)fprintf(stderr, "X11 frontend was not compiled; install X11 development headers or use --headless.\n");
		dpc_display_destroy(display);
		return 2;
	}
#endif
	for (frame = 0U; frame < 60U; ++frame) render(display, frame);
	(void)printf("DISPLAY_SIM_HEADLESS_PASSED frames=%u\n", frame);
	dpc_display_destroy(display);
	return 0;
}
