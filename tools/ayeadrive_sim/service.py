# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import json
import hashlib
import sys
from dataclasses import dataclass, field
from typing import Any

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

@dataclass
class Device:
	family: str
	state: dict[str, Any] = field(default_factory=lambda: {"started": False, "events": [], "faults": []})

	def handle(self, request: dict[str, Any]) -> dict[str, Any]:
		operation = request.get("operation")
		if operation == "start":
			self.state["started"] = True
			return self.result(operation, "accepted")
		if operation == "stop":
			self.state["started"] = False
			return self.result(operation, "accepted")
		if operation == "reset":
			self.state = {"started": self.state["started"], "events": [], "faults": []}
			return self.result(operation, "accepted")
		if operation == "fault":
			fault = request.get("fault")
			if not isinstance(fault, str) or not fault:
				return self.result(operation, "invalid_request", error="fault must be non-empty")
			self.state["faults"].append(fault)
			return self.result(operation, "accepted")
		if operation == "exercise":
			return self.exercise(request)
		if not self.state["started"]:
			return self.result(operation, "not_ready")
		if operation == "write":
			value = request.get("value")
			if not isinstance(value, (int, float, str, bool, list, dict)):
				return self.result(operation, "invalid_request", error="value type unsupported")
			key = request.get("key", "value")
			if not isinstance(key, str) or not key:
				return self.result(operation, "invalid_request", error="key must be non-empty")
			self.state[key] = value
			self.state["events"].append({"operation": operation, "key": key})
			return self.result(operation, "accepted", key=key)
		if operation == "read":
			key = request.get("key", "value")
			if key not in self.state:
				return self.result(operation, "not_found", key=key)
			return self.result(operation, "accepted", key=key, value=self.state[key])
		if operation == "advance":
			duration = request.get("nanoseconds")
			if not isinstance(duration, int) or duration < 0:
				return self.result(operation, "invalid_request", error="nanoseconds must be non-negative integer")
			now = self.state.get("time_ns", 0)
			if now > (2**64 - 1) - duration:
				return self.result(operation, "device_error", error="time overflow")
			self.state["time_ns"] = now + duration
			return self.result(operation, "accepted", time_ns=self.state["time_ns"])
		return self.result(operation, "invalid_request", error="unsupported operation")

	def exercise(self, request: dict[str, Any]) -> dict[str, Any]:
		expected = FAMILY_EXERCISES[self.family]
		action = request.get("action")
		if action != expected:
			return self.result("exercise", "invalid_request", expected_action=expected)
		payload = request.get("payload", {})
		if not isinstance(payload, dict):
			return self.result("exercise", "invalid_request", error="payload must be an object")
		if expected == "digest":
			value = payload.get("data")
			if not isinstance(value, str):
				return self.result("exercise", "invalid_request", error="data must be text")
			return self.result("exercise", "accepted", digest=hashlib.sha256(value.encode("utf-8")).hexdigest())
		if expected == "sample" and not isinstance(payload.get("channel", 0), int):
			return self.result("exercise", "invalid_request", error="channel must be integer")
		if expected == "io" and payload.get("length", 0) not in range(1, 4097):
			return self.result("exercise", "invalid_request", error="length out of range")
		self.state["last_exercise"] = {"action": action, "payload": payload}
		self.state["events"].append({"operation": "exercise", "action": action})
		return self.result("exercise", "accepted", action=action)

	def result(self, operation: Any, status: str, **extra: Any) -> dict[str, Any]:
		return {"sim": FAMILIES[self.family], "family": self.family, "operation": operation, "status": status, **extra}


def run(stream: Any = sys.stdin, output: Any = sys.stdout) -> None:
	devices: dict[str, Device] = {}
	for line in stream:
		if not line.strip():
			continue
		request = json.loads(line)
		family = request.get("family")
		if family not in FAMILIES:
			response = {"status": "invalid_request", "error": "unknown family", "family": family}
		else:
			device = devices.setdefault(family, Device(family))
			response = device.handle(request)
		output.write(json.dumps(response, sort_keys=True) + "\n")
		output.flush()


if __name__ == "__main__":
	run()
