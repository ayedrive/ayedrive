# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import json
import tempfile
import unittest
from pathlib import Path

from tools.ayeadrive_sim.harness import run
from tools.ayeadrive_sim.service import FAMILIES


class AyeDriveHarnessTests(unittest.TestCase):
	def test_all_families_have_independent_audit_results(self) -> None:
		with tempfile.TemporaryDirectory() as directory:
			self.assertEqual(0, run(Path(directory)))
			report = json.loads((Path(directory) / "ayeadrive-simulation-report.json").read_text(encoding="utf-8"))
		self.assertEqual("AyeDrive", report["product"])
		self.assertEqual(set(FAMILIES), {item["family"] for item in report["families"]})
		self.assertTrue(all(item["passed"] for item in report["families"]))
		self.assertEqual("not_claimed", report["certification"])


if __name__ == "__main__":
	unittest.main()
