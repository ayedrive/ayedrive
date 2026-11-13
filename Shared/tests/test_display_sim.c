/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "display_sim.h"

#include <stdio.h>

static int check(int condition, const char *name)
{
	if (!condition) {
		(void)fprintf(stderr, "FAIL: %s\n", name);
		return 0;
	}
	(void)printf("PASS: %s\n", name);
	return 1;
}

int main(void)
{
	dpc_display *display = dpc_display_create();
	dpc_display_mode mode = {640U, 480U, 60U};
	dpc_display_rect rect = {10U, 20U, 100U, 50U};
	dpc_display_color red = {255U, 0U, 0U, 255U};
	dpc_display_color blue = {0U, 0U, 255U, 255U};
	dpc_u64 fence = 0U;
	const dpc_u8 *pixels = NULL;
	dpc_size_t length = 0U;
	const dpc_hal_ops *hal;
	dpc_display_capabilities capabilities = {0};
	int result = 1;
	if (display == NULL) return 1;
	result &= check(dpc_display_start(display) == DPC_HAL_ACCEPTED, "display starts");
	result &= check(dpc_display_set_mode(display, mode) == DPC_HAL_ACCEPTED, "mode negotiation");
	result &= check(dpc_display_clear(display, blue) == DPC_HAL_ACCEPTED, "framebuffer clear");
	result &= check(dpc_display_fill_rect(display, rect, red) == DPC_HAL_ACCEPTED, "rectangle draw");
	result &= check(dpc_display_present(display, &fence) == DPC_HAL_ACCEPTED && fence == 1U, "present fence");
	result &= check(dpc_display_wait_fence(display, fence) == DPC_HAL_ACCEPTED, "fence completion");
	result &= check(dpc_display_framebuffer(display, &pixels, &length) == DPC_HAL_ACCEPTED && length == 640U * 480U * 4U, "framebuffer export");
	result &= check(dpc_display_inject_fault(display) == DPC_HAL_ACCEPTED && dpc_display_clear(display, blue) == DPC_HAL_DEVICE_ERROR, "fault transition");
	result &= check(dpc_display_reset(display) == DPC_HAL_ACCEPTED && dpc_display_get_state(display) == DPC_DISPLAY_MODE_SET, "reset recovery");
	result &= check(dpc_display_get_capabilities(display, &capabilities) == DPC_HAL_ACCEPTED && capabilities.selected_backend == DPC_DISPLAY_BACKEND_SOFTWARE, "acceleration capability report");
	result &= check(dpc_display_select_backend(display, DPC_DISPLAY_BACKEND_CUDA) == DPC_HAL_NOT_SUPPORTED || capabilities.cuda != 0U, "CUDA requires initialized backend");
	result &= check(dpc_display_select_backend(display, DPC_DISPLAY_BACKEND_OPENCL) == DPC_HAL_NOT_SUPPORTED || capabilities.opencl != 0U, "OpenCL requires initialized backend");
	hal = dpc_display_hal(display);
	result &= check(hal != NULL && hal->abi_version == DPC_HAL_ABI_VERSION && hal->context == display, "HAL binding metadata");
	dpc_display_destroy(display);
	return result ? 0 : 1;
}
