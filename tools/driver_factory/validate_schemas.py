# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import json
from pathlib import Path

from jsonschema import Draft202012Validator

ROOT = Path(__file__).resolve().parents[2]
MODELS = ROOT / "DriverTest" / "models"


def main() -> int:
	pairs = [
		("device-spec.schema.json", "device-spec.example.json"),
		("device-dossier.schema.json", "device-dossier.template.json"),
	]
	for schema_name, document_name in pairs:
		schema = json.loads((MODELS / schema_name).read_text(encoding="utf-8"))
		document = json.loads((MODELS / document_name).read_text(encoding="utf-8"))
		Draft202012Validator.check_schema(schema)
		Draft202012Validator(schema).validate(document)
		print(f"SCHEMA_VALID {schema_name} {document_name}")
	registry = json.loads((MODELS / "adapter-capabilities.json").read_text(encoding="utf-8"))
	if registry.get("schemaVersion") != 1 or not registry.get("adapters"):
		raise ValueError("adapter capability registry is empty or has unsupported schemaVersion")
	print(f"REGISTRY_VALID adapters={len(registry['adapters'])}")
	return 0


if __name__ == "__main__":
	raise SystemExit(main())
