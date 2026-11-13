/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "display_sim.h"

static dpc_display *g_display;
static dpc_display_mode g_mode = {640U, 480U, 60U};
static dpc_u32 g_frame;

static void paint_frame(HDC dc, const RECT *client)
{
	const dpc_u8 *pixels = NULL;
	dpc_size_t length = 0U;
	BITMAPINFO bitmap = {0};
	if (dpc_display_framebuffer(g_display, &pixels, &length) != DPC_HAL_ACCEPTED || pixels == NULL) return;
	bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
	bitmap.bmiHeader.biWidth = (LONG)g_mode.width;
	bitmap.bmiHeader.biHeight = -(LONG)g_mode.height;
	bitmap.bmiHeader.biPlanes = 1;
	bitmap.bmiHeader.biBitCount = 32;
	bitmap.bmiHeader.biCompression = BI_RGB;
	StretchDIBits(dc, 0, 0, client->right - client->left, client->bottom - client->top,
		0, 0, (int)g_mode.width, (int)g_mode.height, pixels, &bitmap, DIB_RGB_COLORS, SRCCOPY);
}

static void render_frame(HWND window)
{
	dpc_display_rect bar = {0U, 0U, 160U, 480U};
	dpc_display_rect moving = {(g_frame * 4U) % 540U, 180U, 100U, 100U};
	dpc_display_color background = {12U, 18U, 32U, 255U};
	dpc_display_color accent = {30U, 180U, 255U, 255U};
	dpc_display_color panel = {24U, 32U, 55U, 255U};
	dpc_u64 fence = 0U;
	(void)dpc_display_clear(g_display, background);
	(void)dpc_display_fill_rect(g_display, bar, panel);
	(void)dpc_display_fill_rect(g_display, moving, accent);
	if (dpc_display_present(g_display, &fence) == DPC_HAL_ACCEPTED && dpc_display_wait_fence(g_display, fence) == DPC_HAL_ACCEPTED) {
		InvalidateRect(window, NULL, FALSE);
	}
	++g_frame;
}

static LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	if (message == WM_CREATE) {
		SetTimer(window, 1U, 16U, NULL);
		return 0;
	}
	if (message == WM_TIMER) {
		render_frame(window);
		return 0;
	}
	if (message == WM_PAINT) {
		PAINTSTRUCT paint;
		RECT client;
		HDC dc = BeginPaint(window, &paint);
		GetClientRect(window, &client);
		paint_frame(dc, &client);
		EndPaint(window, &paint);
		return 0;
	}
	if (message == WM_DESTROY) {
		KillTimer(window, 1U);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcA(window, message, wparam, lparam);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show)
{
	WNDCLASSA window_class = {0};
	HWND window;
	MSG message;
	(void)previous;
	(void)command_line;
	g_display = dpc_display_create();
	if (g_display == NULL || dpc_display_start(g_display) != DPC_HAL_ACCEPTED || dpc_display_set_mode(g_display, g_mode) != DPC_HAL_ACCEPTED) return 1;
	window_class.lpfnWndProc = window_proc;
	window_class.hInstance = instance;
	window_class.lpszClassName = "AyeDriveDisplaySimulator";
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClassA(&window_class);
	window = CreateWindowExA(0, window_class.lpszClassName, "AyeDrive C Display Simulator", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 960, 720, NULL, NULL, instance, NULL);
	if (window == NULL) return 1;
	ShowWindow(window, show);
	render_frame(window);
	while (GetMessage(&message, NULL, 0, 0) > 0) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
	dpc_display_stop(g_display);
	dpc_display_destroy(g_display);
	return (int)message.wParam;
}
