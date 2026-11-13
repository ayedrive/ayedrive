# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import argparse
import hashlib
import json
import sys
import csv
from pathlib import Path
from typing import Any

from .provenance import build_manifest, verify_manifest


FUNCTIONAL_CLASSES = {
	"char_control",
	"block_storage",
	"network_fieldbus",
	"continuous_stream",
	"actuator_motion_timing",
	"bus_controller_bridge",
}


def dossier_package(dossier: dict[str, Any], out: Path) -> None:
	out.mkdir(parents=True, exist_ok=True)
	normalized = json.dumps(dossier, indent=2, sort_keys=True) + "\n"
	(out / "dossier.normalized.json").write_text(normalized, encoding="utf-8")
	(out / "intake-report.json").write_text(json.dumps(report(dossier), indent=2, sort_keys=True) + "\n", encoding="utf-8")
	targets = ", ".join(dossier["targets"])
	architecture = (
		"flowchart LR\n"
		f"  OS[OS adapters: {targets}] --> CORE[Portable device core]\n"
		"  CORE --> HAL[Versioned HAL contract]\n"
		"  HAL --> SIM[Deterministic simulator: profile required]\n"
		"  HAL --> HW[Physical backend: not established]\n"
	)
	(out / "architecture.mmd").write_text(architecture, encoding="utf-8")
	with (out / "hazard-review-prompts.csv").open("w", encoding="utf-8", newline="") as stream:
		writer = csv.writer(stream, lineterminator="\n")
		writer.writerow(["id", "interface", "review_prompt", "assessment_status", "evidence_reference"])
		for index, interface in enumerate(dossier["interfaces"], start=1):
			interface_name = f"{interface['transport']}:{interface['functionalClass']}"
			for suffix, prompt in (
				("IO", "Assess malformed, unauthorized, excessive, reordered, and stale I/O"),
				("IRQ", "Assess interrupt loss, storm, latency, context split, and recovery"),
				("DMA", "Assess address bounds, ownership, IOMMU, cache sync, and corruption"),
				("LIFE", "Assess reset, power transition, hot removal, and restart behavior"),
			):
				writer.writerow([f"PROMPT-{index:02d}-{suffix}", interface_name, prompt, "open_not_assessed", ""])
	trace = [
		{"requirement": "DOSSIER-IDENTITY", "sourceReferences": dossier["evidence"]["sourceDocuments"], "status": "open"},
		{"requirement": "DOSSIER-PROTOCOL", "sourceReferences": dossier["device"]["protocolReferences"], "status": "open"},
		{"requirement": "DOSSIER-GOLDEN-TRACE", "sourceReferences": dossier["evidence"]["goldenTraces"], "status": "open"},
		{"requirement": "DOSSIER-PROVENANCE", "sourceReferences": dossier["evidence"]["provenance"], "status": "open"},
	]
	(out / "traceability.json").write_text(json.dumps(trace, indent=2, sort_keys=True) + "\n", encoding="utf-8")
TRANSFERS = {"pio", "irq_fifo", "coherent_dma", "scatter_gather_dma"}
BEHAVIOR_AXES = {"transfer", "cacheCoherency", "concurrency", "lifecycle", "power", "timing", "errorRecovery"}
DOSSIER_REQUIRED = {
	"schemaVersion",
	"dossierId",
	"status",
	"device",
	"interfaces",
	"behavior",
	"targets",
	"evidence",
	"safety",
}
DEVICE_REQUIRED = {"manufacturer", "model", "hardwareRevision", "identifiers", "firmware", "protocolReferences"}
INTERFACE_REQUIRED = {"transport", "functionalClass", "enumeration", "registersOrEndpoints", "interrupts", "dma"}
EVIDENCE_REQUIRED = {"sourceDocuments", "goldenTraces", "provenance", "missingItems"}
ASSURANCE_STATUSES = {"planning", "evidence_in_progress", "review_ready", "accepted_for_testing", "approved", "rejected"}


def _require_object(value: Any, label: str) -> dict[str, Any]:
	if not isinstance(value, dict):
		raise ValueError(f"{label} must be a JSON object")
	return value


def _require_keys(value: dict[str, Any], required: set[str], label: str) -> None:
	missing = sorted(required - value.keys())
	if missing:
		raise ValueError(f"{label} missing required keys: {', '.join(missing)}")
	extra = sorted(value.keys() - required)
	if extra:
		raise ValueError(f"{label} has unknown keys: {', '.join(extra)}")


def validate_dossier(data: Any) -> dict[str, Any]:
	dossier = _require_object(data, "dossier")
	_require_keys(dossier, DOSSIER_REQUIRED, "dossier")
	if dossier["schemaVersion"] != 1:
		raise ValueError("schemaVersion must be 1")
	if dossier["status"] not in {"intake", "awaiting_evidence", "review_ready", "accepted_for_modeling", "rejected"}:
		raise ValueError("status is not a supported dossier state")

	device = _require_object(dossier["device"], "device")
	_require_keys(device, DEVICE_REQUIRED, "device")
	for key in ("manufacturer", "model", "hardwareRevision", "firmware"):
		if not isinstance(device[key], str) or not device[key].strip():
			raise ValueError(f"device.{key} must be a non-empty string")
	if not isinstance(device["identifiers"], list) or not device["identifiers"]:
		raise ValueError("device.identifiers must be a non-empty array")
	if not isinstance(device["protocolReferences"], list) or not device["protocolReferences"]:
		raise ValueError("device.protocolReferences must be a non-empty array")

	interfaces = dossier["interfaces"]
	if not isinstance(interfaces, list) or not interfaces:
		raise ValueError("interfaces must be a non-empty array")
	for index, value in enumerate(interfaces):
		interface = _require_object(value, f"interfaces[{index}]")
		_require_keys(interface, INTERFACE_REQUIRED, f"interfaces[{index}]")
		if interface["functionalClass"] not in FUNCTIONAL_CLASSES:
			raise ValueError(f"interfaces[{index}].functionalClass is not supported")

	behavior = _require_object(dossier["behavior"], "behavior")
	_require_keys(behavior, BEHAVIOR_AXES, "behavior")
	if not isinstance(behavior["transfer"], list) or not behavior["transfer"]:
		raise ValueError("behavior.transfer must be a non-empty array")
	if any(item not in TRANSFERS for item in behavior["transfer"]):
		raise ValueError("behavior.transfer contains an unsupported transfer mode")

	if not isinstance(dossier["targets"], list) or not dossier["targets"]:
		raise ValueError("targets must be a non-empty array")
	evidence = _require_object(dossier["evidence"], "evidence")
	_require_keys(evidence, EVIDENCE_REQUIRED, "evidence")
	safety = _require_object(dossier["safety"], "safety")
	if set(safety) != {"role", "profileReference"}:
		raise ValueError("safety must contain only role and profileReference")
	if safety["role"] not in {"non_safety", "monitor_only", "control", "monitoring_and_control", "undetermined"}:
		raise ValueError("safety.role is unsupported")
	if dossier["status"] == "accepted_for_modeling":
		missing_report = report(dossier)
		if missing_report["missingItems"]:
			raise ValueError("accepted_for_modeling requires complete device facts and evidence")
	return dossier


def load_json(path: Path) -> Any:
	try:
		return json.loads(path.read_text(encoding="utf-8"))
	except (OSError, json.JSONDecodeError) as error:
		raise ValueError(f"cannot read valid JSON from {path}: {error}") from error


def validate_assurance_package(data: Any) -> dict[str, Any]:
	package = _require_object(data, "assurance package")
	required = {"schemaVersion", "packageId", "status", "boundary", "requirements", "targetHardware", "simulators", "reviews", "processAssumptions", "standardsAssumptions", "testCases"}
	required |= {"product", "coverageContract", "standaloneHalMatrix", "sampleDeviceInventory"}
	_require_keys(package, required, "assurance package")
	if package["schemaVersion"] != 1 or package["status"] not in ASSURANCE_STATUSES:
		raise ValueError("assurance package version or status is unsupported")
	if package["product"] != "AyeDrive" or package["coverageContract"] != "DriverTest/models/device-coverage-contract.json" or package["standaloneHalMatrix"] != "DriverTest/models/standalone-hal-os-matrix.json" or package["sampleDeviceInventory"] != "DriverTest/models/sample-device-inventory.json":
		raise ValueError("assurance package must identify AyeDrive, coverage, HAL matrix, and sample inventory")
	if "certified" in package["boundary"].lower() or "approved" in package["boundary"].lower():
		raise ValueError("assurance package boundary must not claim certification or approval")
	requirement_ids = {item["id"] for item in package["requirements"]}
	if len(requirement_ids) != len(package["requirements"]):
		raise ValueError("assurance requirements must have unique ids")
	test_ids = {item["id"] for item in package["testCases"]}
	if len(test_ids) != len(package["testCases"]):
		raise ValueError("assurance test cases must have unique ids")
	for test_case in package["testCases"]:
		unknown = set(test_case["covers"]) - requirement_ids
		if unknown:
			raise ValueError(f"{test_case['id']} references unknown requirements: {', '.join(sorted(unknown))}")
	for simulator in package["simulators"]:
		if not simulator["realization"].startswith("AyeAI: AyeAM ") or not simulator["realization"].endswith(" sim"):
			raise ValueError(f"{simulator['id']} must use an AyeAI AyeAM realization name")
		unknown = set(simulator["coveredRequirements"]) - requirement_ids
		if unknown:
			raise ValueError(f"{simulator['id']} references unknown requirements: {', '.join(sorted(unknown))}")
	if not any(item["status"] == "planned" for item in package["reviews"]):
		raise ValueError("assurance package requires an unresolved independent review gate")
	if package["status"] == "approved":
		raise ValueError("formal approval is outside this planning validator")
	return package


def report(dossier: dict[str, Any]) -> dict[str, Any]:
	missing = list(dossier["evidence"]["missingItems"])
	for key, value in dossier["device"].items():
		if isinstance(value, str) and (not value.strip() or value.startswith("TBD")):
			missing.append(f"device.{key}")
		if isinstance(value, list) and (not value or any(str(item).startswith("TBD") for item in value)):
			missing.append(f"device.{key}")
	for index, interface in enumerate(dossier["interfaces"]):
		for key, value in interface.items():
			if isinstance(value, str) and (not value.strip() or value.startswith("TBD")):
				missing.append(f"interfaces[{index}].{key}")
	for key, value in dossier["behavior"].items():
		if value == "unknown" or (isinstance(value, str) and (not value.strip() or value.startswith("TBD"))):
			missing.append(f"behavior.{key}")
	for key in ("sourceDocuments", "goldenTraces", "provenance"):
		if not dossier["evidence"][key]:
			missing.append(f"evidence.{key}")
	missing = sorted(set(missing))
	return {
		"dossierId": dossier["dossierId"],
		"status": dossier["status"],
		"missingItems": missing,
		"reviewReady": not missing,
		"safetyApproval": "not_assessed",
		"certification": "not_claimed",
	}


def _hash_file(path: Path) -> str:
	digest = hashlib.sha256()
	with path.open("rb") as stream:
		for chunk in iter(lambda: stream.read(1024 * 1024), b""):
			digest.update(chunk)
	return digest.hexdigest()


def evidence_package(root: Path) -> dict[str, Any]:
	if not root.is_dir():
		raise ValueError(f"evidence directory does not exist: {root}")
	records = []
	for path in sorted(p for p in root.rglob("*") if p.is_file()):
		resolved = path.resolve()
		if root.resolve() not in resolved.parents:
			raise ValueError(f"evidence path escapes root: {path}")
		records.append({"path": path.relative_to(root).as_posix(), "sha256": _hash_file(path), "bytes": path.stat().st_size})
	return {
		"formatVersion": 1,
		"evidenceRoot": root.name,
		"files": records,
		"independentReview": {"status": "open", "reviewer": None, "approvalReference": None},
		"safetyApproval": {"status": "open", "authority": None, "decisionReference": None},
		"certification": "not_claimed",
	}


def main(argv: list[str] | None = None) -> int:
	parser = argparse.ArgumentParser(prog="driver-factory")
	commands = parser.add_subparsers(dest="command", required=True)
	validate = commands.add_parser("dossier-validate", help="validate a device dossier JSON structure")
	validate.add_argument("dossier", type=Path)
	inspect = commands.add_parser("dossier-report", help="report missing dossier evidence")
	inspect.add_argument("dossier", type=Path)
	inspect.add_argument("--out", type=Path)
	generate = commands.add_parser("dossier-package", help="generate deterministic dossier reports and review prompts")
	generate.add_argument("dossier", type=Path)
	generate.add_argument("--out", type=Path, required=True)
	package = commands.add_parser("evidence-index", help="hash evidence files and emit open approval gates")
	package.add_argument("directory", type=Path)
	package.add_argument("--out", type=Path, required=True)
	attest = commands.add_parser("attest", help="create an unsigned SHA-256 inventory of files under a root")
	attest.add_argument("root", type=Path)
	attest.add_argument("--out", type=Path, required=True)
	verify = commands.add_parser("verify-attestation", help="verify hashes and paths in an unsigned inventory")
	verify.add_argument("root", type=Path)
	verify.add_argument("manifest", type=Path)
	assurance = commands.add_parser("assurance-validate", help="validate requirements-to-evidence assurance planning package")
	assurance.add_argument("package", type=Path)
	args = parser.parse_args(argv)
	try:
		if args.command == "assurance-validate":
			package = validate_assurance_package(load_json(args.package))
			print(json.dumps({"status": "valid", "packageId": package["packageId"], "certification": "not_claimed"}, indent=2, sort_keys=True))
		elif args.command in {"dossier-validate", "dossier-report", "dossier-package"}:
			dossier = validate_dossier(load_json(args.dossier))
			if args.command == "dossier-package":
				dossier_package(dossier, args.out)
				print(f"DOSSIER_PACKAGE_WRITTEN {args.out}")
			else:
				result = {"status": "valid", "dossierId": dossier["dossierId"]} if args.command == "dossier-validate" else report(dossier)
				rendered = json.dumps(result, indent=2, sort_keys=True) + "\n"
				if args.command == "dossier-report" and args.out:
					args.out.parent.mkdir(parents=True, exist_ok=True)
					args.out.write_text(rendered, encoding="utf-8")
				else:
					print(rendered, end="")
		elif args.command == "evidence-index":
			result = evidence_package(args.directory)
			args.out.parent.mkdir(parents=True, exist_ok=True)
			args.out.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n", encoding="utf-8")
			print(f"EVIDENCE_INDEX_WRITTEN {args.out}")
		elif args.command == "attest":
			root = args.root.resolve()
			manifest_path = args.out.resolve()
			files = [path for path in root.rglob("*") if path.is_file() and path.resolve() != manifest_path]
			result = build_manifest(root, files)
			args.out.parent.mkdir(parents=True, exist_ok=True)
			args.out.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n", encoding="utf-8")
			print(f"HASH_INVENTORY_CREATED files={len(result['files'])} signature=not_signed")
		else:
			verify_manifest(args.root, load_json(args.manifest))
			print("HASH_INVENTORY_VALID signature=not_signed")
		return 0
	except (OSError, ValueError, TypeError, KeyError) as error:
		print(f"ERROR: {error}", file=sys.stderr)
		return 2


if __name__ == "__main__":
	raise SystemExit(main())
