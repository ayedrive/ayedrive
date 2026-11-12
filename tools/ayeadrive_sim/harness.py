# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from .service import FAMILIES


def cases(family: str) -> list[dict[str, Any]]:
	action = {
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
	}[family]
	payload = {"data": "AyeDrive"} if action == "digest" else ({"length": 128} if action == "io" else {"channel": 0} if action == "sample" else {})
	return [
		{"family": family, "operation": "read", "key": "value"},
		{"family": family, "operation": "start"},
		{"family": family, "operation": "write", "key": "value", "value": 42},
		{"family": family, "operation": "read", "key": "value"},
		{"family": family, "operation": "advance", "nanoseconds": 100},
		{"family": family, "operation": "fault", "fault": "deterministic_fault"},
		{"family": family, "operation": "exercise", "action": action, "payload": payload},
		{"family": family, "operation": "reset"},
		{"family": family, "operation": "stop"},
		{"family": family, "operation": "advance", "nanoseconds": -1},
	]


def run(output_dir: Path, continue_recoverable: bool = True) -> int:
	output_dir.mkdir(parents=True, exist_ok=True)
	started = datetime.now(timezone.utc).isoformat()
	results: list[dict[str, Any]] = []
	failures = 0
	for family in FAMILIES:
		requests = cases(family)
		process = subprocess.Popen(
			[sys.executable, "-m", "tools.ayeadrive_sim.service"],
			stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
			text=True,
		)
		assert process.stdin is not None and process.stdout is not None
		responses: list[dict[str, Any]] = []
		for request in requests:
			process.stdin.write(json.dumps(request) + "\n")
			process.stdin.flush()
			response = json.loads(process.stdout.readline())
			responses.append(response)
		process.stdin.close()
		process.wait(timeout=10)
		process.stdout.close()
		process.stderr.close()
		expected = ["not_ready", "accepted", "accepted", "accepted", "accepted", "accepted", "accepted", "accepted", "accepted", "not_ready"]
		observed = [response.get("status") for response in responses]
		passed = observed == expected and all(response.get("sim", "").startswith("AyeAI: AyeAM ") for response in responses)
		if not passed:
			failures += 1
			if not continue_recoverable:
				break
		results.append({"family": family, "passed": passed, "expected": expected, "observed": observed, "responses": responses})
	report = {"product": "AyeDrive", "started": started, "completed": datetime.now(timezone.utc).isoformat(), "families": results, "failures": failures, "status": "passed" if failures == 0 else "failed", "certification": "not_claimed"}
	(output_dir / "ayeadrive-simulation-report.json").write_text(json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
	(output_dir / "ayeadrive-simulation-report.md").write_text("# AyeDrive simulation audit\n\n" + json.dumps(report, indent=2, sort_keys=True) + "\n", encoding="utf-8")
	print(json.dumps({"status": report["status"], "families": len(results), "failures": failures, "certification": "not_claimed"}))
	return 0 if failures == 0 else 1


def main() -> int:
	parser = argparse.ArgumentParser()
	parser.add_argument("--output", type=Path, default=Path("artifacts/ayeadrive"))
	parser.add_argument("--fail-fast", action="store_true")
	args = parser.parse_args()
	return run(args.output, continue_recoverable=not args.fail_fast)


if __name__ == "__main__":
	raise SystemExit(main())
