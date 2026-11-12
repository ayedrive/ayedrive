/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#include "device_virtual_model.h"

#include <string.h>

static dpc_hal_status model_error(dpc_virtual_model *model, dpc_hal_status status)
{
	if (model != NULL && model->fault_count != UINT64_MAX) {
		model->fault_count += 1U;
	}
	return status;
}

static int range_valid(dpc_u64 offset, dpc_size_t length, dpc_size_t capacity)
{
	return (offset <= (dpc_u64)capacity) && (length <= (capacity - (dpc_size_t)offset));
}

static dpc_hal_status profile_operation(dpc_vm_profile profile, dpc_u32 operation)
{
	dpc_hal_status result = DPC_HAL_NOT_SUPPORTED;
	switch (profile) {
	case DPC_VM_CHAR_CONTROL:
		if (operation == DPC_HAL_REGISTER_READ || operation == DPC_HAL_REGISTER_WRITE) result = DPC_HAL_ACCEPTED;
		break;
	case DPC_VM_BLOCK_STORAGE:
		if (operation == DPC_HAL_BLOCK_READ || operation == DPC_HAL_BLOCK_WRITE) result = DPC_HAL_ACCEPTED;
		break;
	case DPC_VM_NETWORK_FIELDBUS:
		if (operation == DPC_HAL_PACKET_SEND || operation == DPC_HAL_PACKET_RECEIVE) result = DPC_HAL_ACCEPTED;
		break;
	case DPC_VM_CONTINUOUS_STREAM:
		if (operation == DPC_HAL_STREAM_READ || operation == DPC_HAL_DMA_READ) result = DPC_HAL_ACCEPTED;
		break;
	case DPC_VM_ACTUATOR_MOTION_TIMING:
		if (operation == DPC_HAL_REGISTER_READ || operation == DPC_HAL_REGISTER_WRITE) result = DPC_HAL_ACCEPTED;
		break;
	case DPC_VM_BUS_CONTROLLER_BRIDGE:
		if (operation == DPC_HAL_REGISTER_READ || operation == DPC_HAL_REGISTER_WRITE ||
			operation == DPC_HAL_DMA_READ || operation == DPC_HAL_DMA_WRITE) result = DPC_HAL_ACCEPTED;
		break;
	case DPC_VM_TRANSPORT_USB_PCIE_THUNDERBOLT:
	case DPC_VM_HID_INPUT:
	case DPC_VM_AUDIO_VIDEO:
	case DPC_VM_GPU_DISPLAY:
	case DPC_VM_CAMERA_IMAGING:
	case DPC_VM_SERIAL_GPIO_BUSES:
	case DPC_VM_SENSOR_ADC_DAC:
	case DPC_VM_POWER_BATTERY:
	case DPC_VM_SECURE_CRYPTO:
	case DPC_VM_WIRELESS:
	case DPC_VM_STORAGE_FILESYSTEM:
	case DPC_VM_VIRTUALIZATION_PARAVIRTUAL:
	case DPC_VM_IDENTITY_BIOMETRIC:
	case DPC_VM_INDUSTRIAL_MEDICAL_SAFETY:
	case DPC_VM_MULTIFUNCTION_HOTPLUG:
		if (operation == DPC_HAL_REGISTER_READ || operation == DPC_HAL_REGISTER_WRITE ||
			operation == DPC_HAL_PACKET_SEND || operation == DPC_HAL_PACKET_RECEIVE ||
			operation == DPC_HAL_STREAM_READ || operation == DPC_HAL_DMA_READ || operation == DPC_HAL_DMA_WRITE) result = DPC_HAL_ACCEPTED;
		break;
	default:
		result = DPC_HAL_INVALID_REQUEST;
		break;
	}
	return result;
}

void dpc_virtual_model_init(dpc_virtual_model *model, dpc_vm_profile profile, dpc_u64 operation_latency_ns)
{
	if (model != NULL) {
		(void)memset(model, 0, sizeof(*model));
		model->profile = profile;
		model->operation_latency_ns = operation_latency_ns;
		model->stream_period_ns = 1000000U;
	}
}

dpc_hal_status dpc_virtual_model_start(dpc_virtual_model *model)
{
	if (model == NULL || model->profile < DPC_VM_CHAR_CONTROL || model->profile > DPC_VM_MULTIFUNCTION_HOTPLUG) {
		return DPC_HAL_INVALID_REQUEST;
	}
	model->started = 1U;
	return DPC_HAL_ACCEPTED;
}

void dpc_virtual_model_stop(dpc_virtual_model *model)
{
	if (model != NULL) model->started = 0U;
}

dpc_hal_status dpc_virtual_model_reset(dpc_virtual_model *model)
{
	if (model == NULL || model->profile < DPC_VM_CHAR_CONTROL || model->profile > DPC_VM_MULTIFUNCTION_HOTPLUG) return DPC_HAL_INVALID_REQUEST;
	if (model->profile == DPC_VM_CHAR_CONTROL || model->profile == DPC_VM_ACTUATOR_MOTION_TIMING) {
		(void)memset(model->registers, 0, sizeof(model->registers));
	}
	if (model->profile == DPC_VM_BLOCK_STORAGE) (void)memset(model->storage, 0, sizeof(model->storage));
	if (model->profile == DPC_VM_NETWORK_FIELDBUS) {
		model->frame_head = 0U;
		model->frame_count = 0U;
	}
	if (model->profile == DPC_VM_CONTINUOUS_STREAM) {
		model->stream_head = 0U;
		model->stream_count = 0U;
	}
	if (model->profile == DPC_VM_ACTUATOR_MOTION_TIMING) {
		model->actuator_setpoint = 0U;
		model->watchdog_deadline_ns = 0U;
	}
	if (model->profile == DPC_VM_BUS_CONTROLLER_BRIDGE) {
		(void)memset(model->dma_arena, 0, sizeof(model->dma_arena));
		model->dma_endpoint = 0U;
	}
	model->next_fault = DPC_VM_FAULT_NONE;
	return DPC_HAL_ACCEPTED;
}

static dpc_hal_status execute_registers(dpc_virtual_model *model, dpc_hal_request *request)
{
	dpc_size_t length = request->operation == DPC_HAL_REGISTER_WRITE ? request->input_length : request->output_capacity;
	if (length == 0U) return DPC_HAL_INVALID_REQUEST;
	if (model->profile == DPC_VM_ACTUATOR_MOTION_TIMING) {
		if (request->address == 0U && request->operation == DPC_HAL_REGISTER_WRITE) {
			dpc_u32 setpoint;
			if (length != sizeof(setpoint)) return DPC_HAL_INVALID_REQUEST;
			(void)memcpy(&setpoint, request->input, sizeof(setpoint));
			if (setpoint > 1000U) return DPC_HAL_INVALID_REQUEST;
			model->actuator_setpoint = setpoint;
			if (model->now_ns > UINT64_MAX - UINT64_C(100000000)) return DPC_HAL_DEVICE_ERROR;
			model->watchdog_deadline_ns = model->now_ns + UINT64_C(100000000);
			return DPC_HAL_ACCEPTED;
		}
		if (request->address == 0U && request->operation == DPC_HAL_REGISTER_READ) {
			if (request->output_capacity < sizeof(model->actuator_setpoint)) return DPC_HAL_INVALID_REQUEST;
			(void)memcpy(request->output, &model->actuator_setpoint, sizeof(model->actuator_setpoint));
			request->output_length = sizeof(model->actuator_setpoint);
			return DPC_HAL_ACCEPTED;
		}
		return DPC_HAL_INVALID_REQUEST;
	}
	if (!range_valid(request->address, length, sizeof(model->registers))) return DPC_HAL_INVALID_REQUEST;
	if (request->operation == DPC_HAL_REGISTER_WRITE) {
		if (model->read_only != 0U || model->next_fault == DPC_VM_FAULT_READ_ONLY) {
			model->next_fault = DPC_VM_FAULT_NONE;
			return DPC_HAL_NOT_SUPPORTED;
		}
		(void)memcpy(&model->registers[(dpc_size_t)request->address], request->input, length);
	} else {
		if (request->output_capacity < length) return DPC_HAL_INVALID_REQUEST;
		(void)memcpy(request->output, &model->registers[(dpc_size_t)request->address], length);
		request->output_length = length;
	}
	return DPC_HAL_ACCEPTED;
}

static dpc_hal_status execute_block(dpc_virtual_model *model, dpc_hal_request *request)
{
	dpc_size_t bytes;
	dpc_size_t sectors;
	if (request->operation == DPC_HAL_BLOCK_WRITE) {
		if (model->read_only != 0U || model->next_fault == DPC_VM_FAULT_READ_ONLY) {
			model->next_fault = DPC_VM_FAULT_NONE;
			return DPC_HAL_NOT_SUPPORTED;
		}
		if (request->input_length == 0U || request->input_length % DPC_VM_SECTOR_BYTES != 0U) return DPC_HAL_INVALID_REQUEST;
		bytes = request->input_length;
		sectors = bytes / DPC_VM_SECTOR_BYTES;
		if (sectors > DPC_VM_MAX_REQUEST_SECTORS || !range_valid(request->address, sectors, DPC_VM_SECTOR_COUNT)) return DPC_HAL_INVALID_REQUEST;
		if (model->next_fault == DPC_VM_FAULT_PARTIAL_WRITE) {
			(void)memcpy(&model->storage[(dpc_size_t)request->address * DPC_VM_SECTOR_BYTES], request->input, bytes / 2U);
			model->next_fault = DPC_VM_FAULT_NONE;
			return DPC_HAL_DEVICE_ERROR;
		}
		(void)memcpy(&model->storage[(dpc_size_t)request->address * DPC_VM_SECTOR_BYTES], request->input, bytes);
		return DPC_HAL_ACCEPTED;
	}
	if (request->input_length == 0U || request->input_length > DPC_VM_MAX_REQUEST_SECTORS) return DPC_HAL_INVALID_REQUEST;
	sectors = request->input_length;
	bytes = sectors * DPC_VM_SECTOR_BYTES;
	if (!range_valid(request->address, sectors, DPC_VM_SECTOR_COUNT) || request->output_capacity < bytes) return DPC_HAL_INVALID_REQUEST;
	(void)memcpy(request->output, &model->storage[(dpc_size_t)request->address * DPC_VM_SECTOR_BYTES], bytes);
	request->output_length = bytes;
	return DPC_HAL_ACCEPTED;
}

static dpc_hal_status execute_packets(dpc_virtual_model *model, dpc_hal_request *request)
{
	dpc_u32 slot;
	if (request->operation == DPC_HAL_PACKET_SEND) {
		if (request->input_length == 0U || request->input_length > DPC_VM_MAX_FRAME_BYTES) return DPC_HAL_INVALID_REQUEST;
		if (model->next_fault == DPC_VM_FAULT_PACKET_DROP) {
			model->next_fault = DPC_VM_FAULT_NONE;
			if (model->dropped_frames != UINT64_MAX) model->dropped_frames += 1U;
			return DPC_HAL_ACCEPTED;
		}
		if (model->frame_count == DPC_VM_PACKET_QUEUE_FRAMES) return DPC_HAL_DEVICE_ERROR;
		slot = (model->frame_head + model->frame_count) % DPC_VM_PACKET_QUEUE_FRAMES;
		model->frames[slot].length = (dpc_u16)request->input_length;
		(void)memcpy(model->frames[slot].bytes, request->input, request->input_length);
		model->frame_count += 1U;
		return DPC_HAL_ACCEPTED;
	}
	if (model->frame_count == 0U) return DPC_HAL_DEVICE_ERROR;
	if (request->output_capacity < model->frames[model->frame_head].length) return DPC_HAL_INVALID_REQUEST;
	request->output_length = model->frames[model->frame_head].length;
	(void)memcpy(request->output, model->frames[model->frame_head].bytes, request->output_length);
	model->frame_head = (model->frame_head + 1U) % DPC_VM_PACKET_QUEUE_FRAMES;
	model->frame_count -= 1U;
	return DPC_HAL_ACCEPTED;
}

static dpc_hal_status execute_stream(dpc_virtual_model *model, dpc_hal_request *request)
{
	dpc_size_t requested;
	dpc_size_t index;
	if (request->operation == DPC_HAL_DMA_READ) {
		if ((request->output_capacity == 0U) || (request->output_capacity % sizeof(dpc_u32)) != 0U ||
			(request->output_capacity > DPC_VM_STREAM_RING_SAMPLES * sizeof(dpc_u32))) return DPC_HAL_INVALID_REQUEST;
		requested = request->output_capacity / sizeof(dpc_u32);
	} else {
		if (request->input_length == 0U || request->input_length > DPC_VM_STREAM_RING_SAMPLES || request->output_capacity < request->input_length * sizeof(dpc_u32)) return DPC_HAL_INVALID_REQUEST;
		requested = request->input_length;
	}
	if (model->next_fault == DPC_VM_FAULT_STREAM_UNDERRUN) {
		model->next_fault = DPC_VM_FAULT_NONE;
		if (model->stream_underruns != UINT64_MAX) model->stream_underruns += 1U;
		return DPC_HAL_DEVICE_ERROR;
	}
	for (index = 0U; index < requested; ++index) {
		dpc_u32 sample = (dpc_u32)((model->stream_head + index) * 2654435761U);
		(void)memcpy(&request->output[index * sizeof(sample)], &sample, sizeof(sample));
	}
	model->stream_head = (model->stream_head + (dpc_u32)requested) % DPC_VM_STREAM_RING_SAMPLES;
	request->output_length = requested * sizeof(dpc_u32);
	return DPC_HAL_ACCEPTED;
}

static dpc_hal_status execute_dma(dpc_virtual_model *model, dpc_hal_request *request)
{
	dpc_size_t length = request->operation == DPC_HAL_DMA_WRITE ? request->input_length : request->output_capacity;
	if (model->profile == DPC_VM_BUS_CONTROLLER_BRIDGE && request->request_id >= DPC_VM_ENDPOINTS) return DPC_HAL_INVALID_REQUEST;
	if (length == 0U) return DPC_HAL_INVALID_REQUEST;
	if (model->profile == DPC_VM_BUS_CONTROLLER_BRIDGE && (request->address % 16U) != 0U) return DPC_HAL_INVALID_REQUEST;
	if (!range_valid(request->address, length, sizeof(model->dma_arena))) return DPC_HAL_INVALID_REQUEST;
	if (model->next_fault == DPC_VM_FAULT_NEXT_IO) {
		model->next_fault = DPC_VM_FAULT_NONE;
		return DPC_HAL_DEVICE_ERROR;
	}
	if (request->operation == DPC_HAL_DMA_WRITE) {
		if (model->read_only != 0U) return DPC_HAL_NOT_SUPPORTED;
		(void)memcpy(&model->dma_arena[(dpc_size_t)request->address], request->input, length);
	} else {
		(void)memcpy(request->output, &model->dma_arena[(dpc_size_t)request->address], length);
		request->output_length = length;
	}
	return DPC_HAL_ACCEPTED;
}

dpc_hal_status dpc_virtual_model_execute(dpc_virtual_model *model, dpc_hal_request *request)
{
	dpc_hal_status status;
	if (request == NULL) return DPC_HAL_INVALID_REQUEST;
	request->output_length = 0U;
	if (model == NULL || request->output == NULL || request->input == NULL) return DPC_HAL_INVALID_REQUEST;
	if (model->started == 0U) return model_error(model, DPC_HAL_DEVICE_ERROR);
	status = profile_operation(model->profile, request->operation);
	if (status != DPC_HAL_ACCEPTED) return model_error(model, status);
	if (model->next_fault == DPC_VM_FAULT_NEXT_IO) {
		model->next_fault = DPC_VM_FAULT_NONE;
		return model_error(model, DPC_HAL_DEVICE_ERROR);
	}
	if (model->operation_latency_ns > UINT64_MAX - model->now_ns) return model_error(model, DPC_HAL_DEVICE_ERROR);
	model->now_ns += model->operation_latency_ns;
	switch (model->profile) {
	case DPC_VM_CHAR_CONTROL:
	case DPC_VM_ACTUATOR_MOTION_TIMING:
		status = execute_registers(model, request);
		break;
	case DPC_VM_BLOCK_STORAGE:
		status = execute_block(model, request);
		break;
	case DPC_VM_NETWORK_FIELDBUS:
		status = execute_packets(model, request);
		break;
	case DPC_VM_CONTINUOUS_STREAM:
		status = execute_stream(model, request);
		break;
	case DPC_VM_BUS_CONTROLLER_BRIDGE:
		status = (request->operation == DPC_HAL_DMA_READ || request->operation == DPC_HAL_DMA_WRITE) ?
			execute_dma(model, request) : execute_registers(model, request);
		break;
	default:
		status = DPC_HAL_INVALID_REQUEST;
		break;
	}
	if (status != DPC_HAL_ACCEPTED) return model_error(model, status);
	return status;
}

dpc_hal_status dpc_virtual_model_advance(dpc_virtual_model *model, dpc_u64 nanoseconds)
{
	if (model == NULL) return DPC_HAL_INVALID_REQUEST;
	if (nanoseconds > UINT64_MAX - model->now_ns) return model_error(model, DPC_HAL_DEVICE_ERROR);
	model->now_ns += nanoseconds;
	if (model->profile == DPC_VM_ACTUATOR_MOTION_TIMING && model->watchdog_deadline_ns != 0U && model->now_ns >= model->watchdog_deadline_ns) {
		model->actuator_setpoint = 0U;
		model->watchdog_deadline_ns = 0U;
		if (model->watchdog_expirations != UINT64_MAX) model->watchdog_expirations += 1U;
		if (model->next_fault == DPC_VM_FAULT_WATCHDOG_EXPIRE) {
			model->next_fault = DPC_VM_FAULT_NONE;
			return model_error(model, DPC_HAL_DEVICE_ERROR);
		}
	}
	return DPC_HAL_ACCEPTED;
}

void dpc_virtual_model_inject_fault(dpc_virtual_model *model, dpc_vm_fault fault)
{
	if (model != NULL && (int)fault >= (int)DPC_VM_FAULT_NONE && fault <= DPC_VM_FAULT_WATCHDOG_EXPIRE) model->next_fault = fault;
}

static dpc_hal_status hal_start(void *context) { return dpc_virtual_model_start((dpc_virtual_model *)context); }
static dpc_hal_status hal_submit(void *context, dpc_hal_request *request, dpc_hal_completion completion, void *completion_context)
{
	dpc_hal_status status;
	if (request == NULL) return DPC_HAL_INVALID_REQUEST;
	request->output_length = 0U;
	status = dpc_virtual_model_execute((dpc_virtual_model *)context, request);
	if (completion != NULL) completion(completion_context, status, request->output_length);
	return status;
}
static dpc_hal_status hal_cancel(void *context, dpc_u32 request_id) { (void)context; (void)request_id; return DPC_HAL_NOT_SUPPORTED; }
static dpc_hal_status hal_reset(void *context) { return dpc_virtual_model_reset((dpc_virtual_model *)context); }
static void hal_stop(void *context) { dpc_virtual_model_stop((dpc_virtual_model *)context); }
static dpc_u64 hal_time(void *context) { return context == NULL ? 0U : ((dpc_virtual_model *)context)->now_ns; }

dpc_hal_status dpc_virtual_model_hal_init(dpc_virtual_model *model, dpc_hal_ops *operations)
{
	if (model == NULL || operations == NULL) return DPC_HAL_INVALID_REQUEST;
	operations->abi_version = DPC_HAL_ABI_VERSION;
	operations->struct_size = (dpc_u32)sizeof(*operations);
	operations->context = model;
	operations->start = hal_start;
	operations->submit = hal_submit;
	operations->cancel = hal_cancel;
	operations->reset = hal_reset;
	operations->stop = hal_stop;
	operations->monotonic_time_ns = hal_time;
	return DPC_HAL_ACCEPTED;
}
