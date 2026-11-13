# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import json
import sys
from typing import Any

from .inventory import SAMPLES
from .independent_devices import create_device

FAMILIES = {
	"transport_usb_pcie_thunderbolt": "AyeAI: AyeAM transport device sim",
	"hid_input": "AyeAI: AyeAM HID input device sim",
	"audio_video": "AyeAI: AyeAM audio video device sim",
	"gpu_display": "AyeAI: AyeAM GPU display device sim",
	"camera_imaging": "AyeAI: AyeAM camera imaging device sim",
	"serial_gpio_buses": "AyeAI: AyeAM serial GPIO bus device sim",
	"sensor_adc_dac": "AyeAI: AyeAM sensor ADC DAC device sim",
	"power_battery": "AyeAI: AyeAM power battery device sim",
	"secure_crypto": "AyeAI: AyeAM secure crypto device sim",
	"wireless": "AyeAI: AyeAM wireless device sim",
	"storage_filesystem": "AyeAI: AyeAM storage filesystem device sim",
	"virtualization_paravirtual": "AyeAI: AyeAM virtualization paravirtual device sim",
	"identity_biometric": "AyeAI: AyeAM identity biometric device sim",
	"industrial_medical_safety": "AyeAI: AyeAM industrial medical safety device sim",
	"multifunction_hotplug": "AyeAI: AyeAM multifunction hotplug device sim",
}

FAMILY_EXERCISES = {
	"transport_usb_pcie_thunderbolt": "enumerate",
	"hid_input": "report",
	"audio_video": "stream",
	"gpu_display": "command",
	"camera_imaging": "frame",
	"serial_gpio_buses": "transfer",
	"sensor_adc_dac": "sample",
	"power_battery": "telemetry",
	"secure_crypto": "digest",
	"wireless": "link",
	"storage_filesystem": "io",
	"virtualization_paravirtual": "ring",
	"identity_biometric": "authenticate",
	"industrial_medical_safety": "alarm",
	"multifunction_hotplug": "attach",
}

def run(stream: Any = sys.stdin, output: Any = sys.stdout) -> None:
	devices: dict[str, Any] = {}
	for line in stream:
		if not line.strip():
			continue
		request = json.loads(line)
		family = request.get("family")
		sample = request.get("sample")
		if sample in SAMPLES:
			family = SAMPLES[sample]["family"]
		if family not in FAMILIES:
			response = {"status": "invalid_request", "error": "unknown family", "family": family}
		else:
			key = sample or family
			if key not in devices:
				if key not in SAMPLES:
					response = {"status": "invalid_request", "error": "independent sample required", "family": family}
					output.write(json.dumps(response, sort_keys=True) + "\n")
					output.flush()
					continue
				devices[key] = create_device(key, SAMPLES)
			device = devices[key]
			status, extra = device.handle(request)
			response = {"sim": f"AyeAI: AyeAM {device.sample} independent device sim", "family": device.family, "sample": device.sample, "kind": device.kind, "operation": request.get("operation"), "status": status, **extra}
		output.write(json.dumps(response, sort_keys=True) + "\n")
		output.flush()


if __name__ == "__main__":
	run()
