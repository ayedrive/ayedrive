# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import json
import io
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path

from driver_factory.cli import evidence_package, main, report, validate_dossier


ROOT = Path(__file__).resolve().parents[3]
MODELS = ROOT / "DriverTest" / "models"


class DossierTests(unittest.TestCase):
	def test_cli_validate_and_report_commands(self) -> None:
		dossier_path = MODELS / "device-dossier.template.json"
		with redirect_stdout(io.StringIO()) as output:
			self.assertEqual(0, main(["dossier-validate", str(dossier_path)]))
		self.assertIn('"status": "valid"', output.getvalue())

		with tempfile.TemporaryDirectory() as temp_dir:
			report_path = Path(temp_dir) / "nested" / "report.json"
			with redirect_stdout(io.StringIO()):
				self.assertEqual(0, main(["dossier-report", str(dossier_path), "--out", str(report_path)]))
			result = json.loads(report_path.read_text(encoding="utf-8"))
			self.assertFalse(result["reviewReady"])

	def test_cli_package_and_evidence_index_commands(self) -> None:
		dossier_path = MODELS / "device-dossier.template.json"
		with tempfile.TemporaryDirectory() as temp_dir:
			root = Path(temp_dir)
			package_path = root / "package"
			with redirect_stdout(io.StringIO()):
				self.assertEqual(0, main(["dossier-package", str(dossier_path), "--out", str(package_path)]))
			self.assertTrue((package_path / "traceability.json").is_file())

			evidence_dir = root / "evidence"
			evidence_dir.mkdir()
			(evidence_dir / "trace.txt").write_text("reference trace\n", encoding="utf-8")
			index_path = root / "index" / "evidence.json"
			with redirect_stdout(io.StringIO()):
				self.assertEqual(0, main(["evidence-index", str(evidence_dir), "--out", str(index_path)]))
			index = json.loads(index_path.read_text(encoding="utf-8"))
			self.assertEqual("open", index["safetyApproval"]["status"])

	def test_cli_attestation_commands_and_unexpected_file_rejection(self) -> None:
		with tempfile.TemporaryDirectory() as temp_dir:
			root = Path(temp_dir) / "evidence"
			root.mkdir()
			(root / "record.txt").write_text("record\n", encoding="utf-8")
			manifest_path = root / "provenance.manifest.json"
			with redirect_stdout(io.StringIO()):
				self.assertEqual(0, main(["attest", str(root), "--out", str(manifest_path)]))
				self.assertEqual(0, main(["verify-attestation", str(root), str(manifest_path)]))
			(root / "unexpected.txt").write_text("extra\n", encoding="utf-8")
			with redirect_stdout(io.StringIO()):
				self.assertEqual(2, main(["verify-attestation", str(root), str(manifest_path)]))

	def test_cli_invalid_dossier_returns_error_code(self) -> None:
		with tempfile.TemporaryDirectory() as temp_dir:
			path = Path(temp_dir) / "invalid.json"
			path.write_text("{}", encoding="utf-8")
			with redirect_stdout(io.StringIO()):
				self.assertEqual(2, main(["dossier-validate", str(path)]))

	def test_intake_template_validates_and_reports_unknowns(self) -> None:
		data = json.loads((MODELS / "device-dossier.template.json").read_text(encoding="utf-8"))
		dossier = validate_dossier(data)
		result = report(dossier)
		self.assertFalse(result["reviewReady"])
		self.assertIn("device.manufacturer", result["missingItems"])
		self.assertEqual("not_assessed", result["safetyApproval"])

	def test_unknown_key_is_rejected(self) -> None:
		data = json.loads((MODELS / "device-dossier.template.json").read_text(encoding="utf-8"))
		data["invented"] = True
		with self.assertRaisesRegex(ValueError, "unknown keys"):
			validate_dossier(data)

	def test_accepted_dossier_requires_complete_evidence(self) -> None:
		data = json.loads((MODELS / "device-dossier.template.json").read_text(encoding="utf-8"))
		data["status"] = "accepted_for_modeling"
		with self.assertRaisesRegex(ValueError, "complete device facts and evidence"):
			validate_dossier(data)

	def test_evidence_index_hashes_files_and_keeps_approval_open(self) -> None:
		with tempfile.TemporaryDirectory() as temp_dir:
			root = Path(temp_dir)
			(root / "report.txt").write_text("evidence\n", encoding="utf-8")
			result = evidence_package(root)
			self.assertEqual(1, len(result["files"]))
			self.assertEqual(64, len(result["files"][0]["sha256"]))
			self.assertEqual("open", result["independentReview"]["status"])
			self.assertEqual("open", result["safetyApproval"]["status"])
			self.assertEqual("not_claimed", result["certification"])

	def test_adapter_registry_matches_schema(self) -> None:
		from jsonschema import Draft202012Validator

		schema = json.loads((MODELS / "adapter-capabilities.schema.json").read_text(encoding="utf-8"))
		registry = json.loads((MODELS / "adapter-capabilities.json").read_text(encoding="utf-8"))
		Draft202012Validator(schema).validate(registry)
		self.assertGreater(len(registry["adapters"]), 0)


if __name__ == "__main__":
	unittest.main()
