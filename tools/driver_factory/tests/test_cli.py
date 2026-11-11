# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

from driver_factory.cli import evidence_package, report, validate_dossier


ROOT = Path(__file__).resolve().parents[3]
MODELS = ROOT / "DriverTest" / "models"


class DossierTests(unittest.TestCase):
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


if __name__ == "__main__":
	unittest.main()
