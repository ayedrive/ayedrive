/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "display_sim.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

struct dpc_display {
	dpc_display_state state;
	dpc_display_mode mode;
	dpc_u8 *pixels;
	dpc_size_t pixel_bytes;
	dpc_u64 next_fence;
	dpc_u64 completed_fence;
	dpc_u64 now_ns;
	dpc_u8 fault_next;
	dpc_display_backend backend;
	dpc_hal_ops hal;
};

static dpc_hal_status display_check(dpc_display *display)
{
	if (display == NULL || display->state == DPC_DISPLAY_OFF) return DPC_HAL_INVALID_REQUEST;
	if (display->state == DPC_DISPLAY_FAULT) return DPC_HAL_DEVICE_ERROR;
	if (display->fault_next != 0U) {
		display->fault_next = 0U;
		display->state = DPC_DISPLAY_FAULT;
		return DPC_HAL_DEVICE_ERROR;
	}
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_write_framebuffer(dpc_display *display, const dpc_u8 *pixels, dpc_size_t length)
{
	dpc_hal_status status = display_check(display);
	if (status != DPC_HAL_ACCEPTED || pixels == NULL || display->pixels == NULL || length != display->pixel_bytes) {
		return status != DPC_HAL_ACCEPTED ? status : DPC_HAL_INVALID_REQUEST;
	}
	(void)memcpy(display->pixels, pixels, length);
	return DPC_HAL_ACCEPTED;
}

static int rect_valid(const dpc_display *display, dpc_display_rect rect)
{
	return display != NULL && rect.x <= display->mode.width && rect.y <= display->mode.height &&
		rect.width <= display->mode.width - rect.x && rect.height <= display->mode.height - rect.y;
}

static dpc_u32 pixel_value(dpc_display_color color)
{
	return ((dpc_u32)color.alpha << 24U) | ((dpc_u32)color.red << 16U) |
		((dpc_u32)color.green << 8U) | color.blue;
}

static void fill_pixels(dpc_display *display, dpc_display_rect rect, dpc_display_color color)
{
	dpc_u32 y;
	dpc_u32 x;
	dpc_u32 value = pixel_value(color);
	for (y = rect.y; y < rect.y + rect.height; ++y) {
		for (x = rect.x; x < rect.x + rect.width; ++x) {
			dpc_size_t index = ((dpc_size_t)y * display->mode.width + x) * 4U;
			(void)memcpy(&display->pixels[index], &value, sizeof(value));
		}
	}
}

dpc_display *dpc_display_create(void)
{
	dpc_display *display = (dpc_display *)calloc(1U, sizeof(*display));
	if (display != NULL) {
		display->state = DPC_DISPLAY_OFF;
		display->next_fence = 1U;
		display->hal.abi_version = DPC_HAL_ABI_VERSION;
		display->hal.struct_size = (dpc_u32)sizeof(display->hal);
		display->hal.context = display;
	}
	return display;
}

void dpc_display_destroy(dpc_display *display)
{
	if (display != NULL) {
		free(display->pixels);
		free(display);
	}
}

dpc_hal_status dpc_display_start(dpc_display *display)
{
	if (display == NULL) return DPC_HAL_INVALID_REQUEST;
	display->state = DPC_DISPLAY_READY;
	return DPC_HAL_ACCEPTED;
}

void dpc_display_stop(dpc_display *display)
{
	if (display != NULL) display->state = DPC_DISPLAY_OFF;
}

dpc_hal_status dpc_display_reset(dpc_display *display)
{
	if (display == NULL) return DPC_HAL_INVALID_REQUEST;
	if (display->pixels != NULL) (void)memset(display->pixels, 0, display->pixel_bytes);
	display->state = display->mode.width != 0U ? DPC_DISPLAY_MODE_SET : DPC_DISPLAY_READY;
	display->fault_next = 0U;
	display->completed_fence = 0U;
	return DPC_HAL_ACCEPTED;
}

dpc_display_state dpc_display_get_state(const dpc_display *display)
{
	return display == NULL ? DPC_DISPLAY_FAULT : display->state;
}

dpc_hal_status dpc_display_set_mode(dpc_display *display, dpc_display_mode mode)
{
	dpc_size_t bytes;
	dpc_hal_status status = display_check(display);
	if (status != DPC_HAL_ACCEPTED) return status;
	if (mode.width == 0U || mode.height == 0U || mode.refresh_hz == 0U || mode.width > 4096U || mode.height > 2160U) return DPC_HAL_INVALID_REQUEST;
	if ((dpc_size_t)mode.width > (dpc_size_t)-1 / mode.height / 4U) return DPC_HAL_INVALID_REQUEST;
	bytes = (dpc_size_t)mode.width * mode.height * 4U;
	free(display->pixels);
	display->pixels = (dpc_u8 *)calloc(1U, bytes);
	if (display->pixels == NULL) return DPC_HAL_DEVICE_ERROR;
	display->mode = mode;
	display->pixel_bytes = bytes;
	display->state = DPC_DISPLAY_MODE_SET;
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_get_mode(const dpc_display *display, dpc_display_mode *mode)
{
	if (display == NULL || mode == NULL || display->mode.width == 0U) return DPC_HAL_INVALID_REQUEST;
	*mode = display->mode;
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_clear(dpc_display *display, dpc_display_color color)
{
	dpc_display_rect rect = {0U, 0U, 0U, 0U};
	dpc_hal_status status = display_check(display);
	if (status != DPC_HAL_ACCEPTED) return status;
	if (display->pixels == NULL) return DPC_HAL_INVALID_REQUEST;
	rect.width = display->mode.width;
	rect.height = display->mode.height;
	fill_pixels(display, rect, color);
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_fill_rect(dpc_display *display, dpc_display_rect rect, dpc_display_color color)
{
	dpc_hal_status status = display_check(display);
	if (status != DPC_HAL_ACCEPTED) return status;
	if (display->pixels == NULL || !rect_valid(display, rect)) return DPC_HAL_INVALID_REQUEST;
	fill_pixels(display, rect, color);
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_present(dpc_display *display, dpc_u64 *fence)
{
	dpc_hal_status status = display_check(display);
	if (status != DPC_HAL_ACCEPTED) return status;
	if (display->pixels == NULL || fence == NULL) return DPC_HAL_INVALID_REQUEST;
	display->state = DPC_DISPLAY_PRESENTING;
	*fence = display->next_fence++;
	display->completed_fence = *fence;
	display->state = DPC_DISPLAY_MODE_SET;
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_wait_fence(dpc_display *display, dpc_u64 fence)
{
	dpc_hal_status status = display_check(display);
	if (status != DPC_HAL_ACCEPTED) return status;
	if (fence == 0U || fence > display->completed_fence) return DPC_HAL_INVALID_REQUEST;
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_framebuffer(const dpc_display *display, const dpc_u8 **pixels, dpc_size_t *length)
{
	if (display == NULL || pixels == NULL || length == NULL || display->pixels == NULL) return DPC_HAL_INVALID_REQUEST;
	*pixels = display->pixels;
	*length = display->pixel_bytes;
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_inject_fault(dpc_display *display)
{
	if (display == NULL) return DPC_HAL_INVALID_REQUEST;
	display->fault_next = 1U;
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_get_capabilities(const dpc_display *display, dpc_display_capabilities *capabilities)
{
	if (display == NULL || capabilities == NULL) return DPC_HAL_INVALID_REQUEST;
	capabilities->native_graphics = 0U;
	capabilities->cuda = 0U;
	capabilities->opencl = 0U;
	capabilities->selected_backend = (dpc_u8)display->backend;
#if defined(DPC_DISPLAY_NATIVE_BACKEND)
	capabilities->native_graphics = 1U;
#endif
#if defined(DPC_DISPLAY_CUDA_BACKEND)
	capabilities->cuda = 1U;
#endif
#if defined(DPC_DISPLAY_OPENCL_BACKEND)
	capabilities->opencl = 1U;
#endif
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_display_select_backend(dpc_display *display, dpc_display_backend backend)
{
	dpc_display_capabilities capabilities;
	if (display == NULL || backend > DPC_DISPLAY_BACKEND_OPENCL) return DPC_HAL_INVALID_REQUEST;
	(void)dpc_display_get_capabilities(display, &capabilities);
	if ((backend == DPC_DISPLAY_BACKEND_NATIVE && capabilities.native_graphics == 0U) ||
		(backend == DPC_DISPLAY_BACKEND_CUDA && capabilities.cuda == 0U) ||
		(backend == DPC_DISPLAY_BACKEND_OPENCL && capabilities.opencl == 0U)) return DPC_HAL_NOT_SUPPORTED;
	display->backend = backend;
	return DPC_HAL_ACCEPTED;
}

static dpc_u64 display_time(void *context)
{
	dpc_display *display = (dpc_display *)context;
	struct timespec value;
	if (display == NULL) return 0U;
	if (timespec_get(&value, TIME_UTC) == TIME_UTC) display->now_ns = (dpc_u64)value.tv_sec * 1000000000U + (dpc_u64)value.tv_nsec;
	return display->now_ns;
}

const dpc_hal_ops *dpc_display_hal(dpc_display *display)
{
	if (display == NULL) return NULL;
	display->hal.start = (dpc_hal_status (*)(void *))dpc_display_start;
	display->hal.reset = (dpc_hal_status (*)(void *))dpc_display_reset;
	display->hal.stop = (void (*)(void *))dpc_display_stop;
	display->hal.monotonic_time_ns = display_time;
	return &display->hal;
}
