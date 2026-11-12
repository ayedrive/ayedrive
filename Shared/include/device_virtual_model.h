/* SPDX-License-Identifier: GPL-3.0-only
 * Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
 */
#ifndef DEVICE_VIRTUAL_MODEL_H
#define DEVICE_VIRTUAL_MODEL_H

#include "device_hal.h"

#define DPC_VM_REGISTER_BYTES 256U
#define DPC_VM_SECTOR_BYTES 512U
#define DPC_VM_SECTOR_COUNT 64U
#define DPC_VM_MAX_REQUEST_SECTORS 8U
#define DPC_VM_PACKET_QUEUE_FRAMES 32U
#define DPC_VM_MAX_FRAME_BYTES 1500U
#define DPC_VM_STREAM_RING_SAMPLES 1024U
#define DPC_VM_DMA_ARENA_BYTES 4096U
#define DPC_VM_ENDPOINTS 2U

typedef enum dpc_vm_profile {
	DPC_VM_CHAR_CONTROL = 1,
	DPC_VM_BLOCK_STORAGE = 2,
	DPC_VM_NETWORK_FIELDBUS = 3,
	DPC_VM_CONTINUOUS_STREAM = 4,
	DPC_VM_ACTUATOR_MOTION_TIMING = 5,
	DPC_VM_BUS_CONTROLLER_BRIDGE = 6,
	DPC_VM_TRANSPORT_USB_PCIE_THUNDERBOLT = 7,
	DPC_VM_HID_INPUT = 8,
	DPC_VM_AUDIO_VIDEO = 9,
	DPC_VM_GPU_DISPLAY = 10,
	DPC_VM_CAMERA_IMAGING = 11,
	DPC_VM_SERIAL_GPIO_BUSES = 12,
	DPC_VM_SENSOR_ADC_DAC = 13,
	DPC_VM_POWER_BATTERY = 14,
	DPC_VM_SECURE_CRYPTO = 15,
	DPC_VM_WIRELESS = 16,
	DPC_VM_STORAGE_FILESYSTEM = 17,
	DPC_VM_VIRTUALIZATION_PARAVIRTUAL = 18,
	DPC_VM_IDENTITY_BIOMETRIC = 19,
	DPC_VM_INDUSTRIAL_MEDICAL_SAFETY = 20,
	DPC_VM_MULTIFUNCTION_HOTPLUG = 21
} dpc_vm_profile;

typedef enum dpc_vm_fault {
	DPC_VM_FAULT_NONE = 0,
	DPC_VM_FAULT_NEXT_IO,
	DPC_VM_FAULT_READ_ONLY,
	DPC_VM_FAULT_PARTIAL_WRITE,
	DPC_VM_FAULT_PACKET_DROP,
	DPC_VM_FAULT_STREAM_UNDERRUN,
	DPC_VM_FAULT_WATCHDOG_EXPIRE
} dpc_vm_fault;

typedef struct dpc_vm_frame {
	dpc_u16 length;
	dpc_u8 bytes[DPC_VM_MAX_FRAME_BYTES];
} dpc_vm_frame;

typedef struct dpc_virtual_model {
	dpc_vm_profile profile;
	dpc_u64 now_ns;
	dpc_u64 operation_latency_ns;
	dpc_u64 fault_count;
	dpc_u64 dropped_frames;
	dpc_u64 stream_underruns;
	dpc_u64 watchdog_expirations;
	dpc_u64 watchdog_deadline_ns;
	dpc_u32 started;
	dpc_u32 read_only;
	dpc_vm_fault next_fault;
	dpc_u8 registers[DPC_VM_REGISTER_BYTES];
	dpc_u8 storage[DPC_VM_SECTOR_COUNT * DPC_VM_SECTOR_BYTES];
	dpc_vm_frame frames[DPC_VM_PACKET_QUEUE_FRAMES];
	dpc_u32 frame_head;
	dpc_u32 frame_count;
	dpc_u32 stream_head;
	dpc_u32 stream_count;
	dpc_u32 stream_period_ns;
	dpc_u32 actuator_setpoint;
	dpc_u32 dma_endpoint;
	dpc_u8 dma_arena[DPC_VM_DMA_ARENA_BYTES];
} dpc_virtual_model;

void dpc_virtual_model_init(dpc_virtual_model *model, dpc_vm_profile profile, dpc_u64 operation_latency_ns);
dpc_hal_status dpc_virtual_model_start(dpc_virtual_model *model);
void dpc_virtual_model_stop(dpc_virtual_model *model);
dpc_hal_status dpc_virtual_model_reset(dpc_virtual_model *model);
dpc_hal_status dpc_virtual_model_execute(dpc_virtual_model *model, dpc_hal_request *request);
dpc_hal_status dpc_virtual_model_advance(dpc_virtual_model *model, dpc_u64 nanoseconds);
void dpc_virtual_model_inject_fault(dpc_virtual_model *model, dpc_vm_fault fault);

dpc_hal_status dpc_virtual_model_hal_init(dpc_virtual_model *model, dpc_hal_ops *operations);

#endif
