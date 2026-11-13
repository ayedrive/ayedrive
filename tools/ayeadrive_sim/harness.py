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

from .inventory import SAMPLES
from .functional_behavior import FUNCTIONS
from .independent_devices import create_device


def cases(sample: str) -> list[dict[str, Any]]:
	family = SAMPLES[sample]["family"]
	action = FUNCTIONS[family]["operation"]
	payload = dict(FUNCTIONS[family]["required"])
	return [
		{"family": family, "sample": sample, "operation": "read", "key": "value"},
		{"family": family, "sample": sample, "operation": "start"},
		{"family": family, "sample": sample, "operation": "write", "key": "value", "value": 42},
		{"family": family, "sample": sample, "operation": "read", "key": "value"},
		{"family": family, "sample": sample, "operation": "advance", "nanoseconds": 100},
		{"family": family, "sample": sample, "operation": "fault", "fault": "deterministic_fault"},
		{"family": family, "sample": sample, "operation": "exercise", "action": action, "payload": payload},
		{"family": family, "sample": sample, "operation": "reset"},
		{"family": family, "sample": sample, "operation": "stop"},
		{"family": family, "sample": sample, "operation": "advance", "nanoseconds": -1},
	]


def run(output_dir: Path, continue_recoverable: bool = True) -> int:
	output_dir.mkdir(parents=True, exist_ok=True)
	started = datetime.now(timezone.utc).isoformat()
	results: list[dict[str, Any]] = []
	failures = 0
	for sample in SAMPLES:
		independent = create_device(sample, SAMPLES)
		requests = cases(sample)
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
		functional = responses[6]
		passed = (
			observed[:6] == expected[:6]
			and observed[6] == "device_error"
			and observed[7:] == expected[7:]
			and all(response.get("sim", "").startswith("AyeAI: AyeAM ") for response in responses)
			and all(response.get("sample") == sample for response in responses)
			and responses[8].get("functional") is True
			and type(independent).__name__.endswith("Simulator")
			and independent.sample == sample
			and type(independent).__module__ == f"tools.ayeadrive_sim.devices.{sample}.simulator"
		)
		if not passed:
			failures += 1
			if not continue_recoverable:
				break
		results.append({"sample": sample, "family": SAMPLES[sample]["family"], "kind": SAMPLES[sample]["kind"], "passed": passed, "expected": expected, "observed": observed, "responses": responses})
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
