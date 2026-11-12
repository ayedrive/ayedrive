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
		("adapter-capabilities.schema.json", "adapter-capabilities.json"),
		("virtual-device-profiles.schema.json", "virtual-device-profiles.json"),
		("assurance-package.schema.json", "assurance-package.json"),
		("device-coverage-contract.schema.json", "device-coverage-contract.json"),
	]
	for schema_name, document_name in pairs:
		schema = json.loads((MODELS / schema_name).read_text(encoding="utf-8"))
		document = json.loads((MODELS / document_name).read_text(encoding="utf-8"))
		Draft202012Validator.check_schema(schema)
		Draft202012Validator(schema).validate(document)
		print(f"SCHEMA_VALID {schema_name} {document_name}")
	return 0


if __name__ == "__main__":
	raise SystemExit(main())
