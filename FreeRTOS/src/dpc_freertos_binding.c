/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "dpc_freertos_binding.h"

#define DPC_FREERTOS_QUEUE_LENGTH 4u
#define DPC_FREERTOS_TASK_STACK_DEPTH (configMINIMAL_STACK_SIZE * 2u)

typedef struct dpc_queued_request {
	dpc_freertos_request request;
} dpc_queued_request;

static StaticQueue_t dpc_queue_control;
static dpc_u8 dpc_queue_storage[DPC_FREERTOS_QUEUE_LENGTH * sizeof(dpc_queued_request)];
static StaticTask_t dpc_task_control;
static StackType_t dpc_task_stack[DPC_FREERTOS_TASK_STACK_DEPTH];
static QueueHandle_t dpc_queue;

static void dpc_worker(void *context)
{
	dpc_queued_request queued;
	dpc_size_t response_length;
	dpc_core_status status;

	(void)context;
	for (;;) {
		if (xQueueReceive(dpc_queue, &queued, portMAX_DELAY) != pdPASS) {
			continue;
		}
		status = dpc_process_message(
			queued.request.input,
			queued.request.input_length,
			queued.request.output,
			queued.request.output_capacity,
			&response_length);
		if (queued.request.completion != NULL) {
			queued.request.completion(queued.request.completion_context, status, response_length);
		}
	}
}

int dpc_freertos_start(void)
{
	if (dpc_queue != NULL) {
		return 0;
	}
	dpc_queue = xQueueCreateStatic(
		DPC_FREERTOS_QUEUE_LENGTH,
		sizeof(dpc_queued_request),
		dpc_queue_storage,
		&dpc_queue_control);
	if (dpc_queue == NULL) {
		return -1;
	}
	if (xTaskCreateStatic(
		dpc_worker,
		"dpc-protocol",
		DPC_FREERTOS_TASK_STACK_DEPTH,
		NULL,
		tskIDLE_PRIORITY + 1u,
		dpc_task_stack,
		&dpc_task_control) == NULL) {
		dpc_queue = NULL;
		return -1;
	}
	return 0;
}

int dpc_freertos_submit(const dpc_freertos_request *request)
{
	dpc_queued_request queued;
	if (dpc_queue == NULL || request == NULL || request->input == NULL ||
		request->output == NULL || request->completion == NULL) {
		return -1;
	}
	queued.request = *request;
	return xQueueSend(dpc_queue, &queued, 0) == pdPASS ? 0 : -1;
}
