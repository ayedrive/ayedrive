/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DISPLAY_SIM_H
#define DISPLAY_SIM_H

#include "device_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dpc_display dpc_display;
typedef struct dpc_display_mode {
	dpc_u32 width;
	dpc_u32 height;
	dpc_u32 refresh_hz;
} dpc_display_mode;

typedef enum dpc_display_state {
	DPC_DISPLAY_OFF = 0,
	DPC_DISPLAY_READY = 1,
	DPC_DISPLAY_MODE_SET = 2,
	DPC_DISPLAY_PRESENTING = 3,
	DPC_DISPLAY_FAULT = 4
} dpc_display_state;

typedef enum dpc_display_command {
	DPC_DISPLAY_CLEAR = 1,
	DPC_DISPLAY_FILL_RECT = 2,
	DPC_DISPLAY_PRESENT = 3
} dpc_display_command;

typedef enum dpc_display_backend {
	DPC_DISPLAY_BACKEND_SOFTWARE = 0,
	DPC_DISPLAY_BACKEND_NATIVE = 1,
	DPC_DISPLAY_BACKEND_CUDA = 2,
	DPC_DISPLAY_BACKEND_OPENCL = 3
} dpc_display_backend;

typedef struct dpc_display_capabilities {
	dpc_u8 native_graphics;
	dpc_u8 cuda;
	dpc_u8 opencl;
	dpc_u8 selected_backend;
} dpc_display_capabilities;

typedef struct dpc_display_rect {
	dpc_u32 x;
	dpc_u32 y;
	dpc_u32 width;
	dpc_u32 height;
} dpc_display_rect;

typedef struct dpc_display_color {
	dpc_u8 red;
	dpc_u8 green;
	dpc_u8 blue;
	dpc_u8 alpha;
} dpc_display_color;

dpc_display *dpc_display_create(void);
void dpc_display_destroy(dpc_display *display);
dpc_hal_status dpc_display_start(dpc_display *display);
void dpc_display_stop(dpc_display *display);
dpc_hal_status dpc_display_reset(dpc_display *display);
dpc_display_state dpc_display_get_state(const dpc_display *display);
dpc_hal_status dpc_display_set_mode(dpc_display *display, dpc_display_mode mode);
dpc_hal_status dpc_display_get_mode(const dpc_display *display, dpc_display_mode *mode);
dpc_hal_status dpc_display_clear(dpc_display *display, dpc_display_color color);
dpc_hal_status dpc_display_fill_rect(dpc_display *display, dpc_display_rect rect, dpc_display_color color);
dpc_hal_status dpc_display_present(dpc_display *display, dpc_u64 *fence);
dpc_hal_status dpc_display_wait_fence(dpc_display *display, dpc_u64 fence);
dpc_hal_status dpc_display_framebuffer(const dpc_display *display, const dpc_u8 **pixels, dpc_size_t *length);
dpc_hal_status dpc_display_write_framebuffer(dpc_display *display, const dpc_u8 *pixels, dpc_size_t length);
dpc_hal_status dpc_display_inject_fault(dpc_display *display);
dpc_hal_status dpc_display_get_capabilities(const dpc_display *display, dpc_display_capabilities *capabilities);
dpc_hal_status dpc_display_select_backend(dpc_display *display, dpc_display_backend backend);
const dpc_hal_ops *dpc_display_hal(dpc_display *display);

#ifdef __cplusplus
}
#endif
#endif
