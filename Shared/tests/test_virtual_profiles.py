# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import json
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
MODELS = ROOT / "DriverTest" / "models"


class VirtualProfileTests(unittest.TestCase):
	def test_six_profiles_match_taxonomy_and_have_nonclaims(self) -> None:
		profiles = json.loads((MODELS / "virtual-device-profiles.json").read_text(encoding="utf-8"))
		taxonomy = json.loads((MODELS / "device-taxonomy.json").read_text(encoding="utf-8"))
		expected = {item["id"] for item in taxonomy["functionalClasses"]}
		actual = {profile["functionalClass"] for profile in profiles["profiles"]}
		self.assertEqual(expected, actual)
		self.assertEqual(6, len(profiles["profiles"]))
		self.assertEqual(6, len({profile["id"] for profile in profiles["profiles"]}))
		for profile in profiles["profiles"]:
			self.assertTrue(profile["id"].startswith("synthetic_"))
			self.assertTrue(profile["supportedOperations"])
			self.assertTrue(profile["faults"])
			self.assertTrue(profile["nonClaims"])
		self.assertIn("not physical", profiles["boundary"].lower())


if __name__ == "__main__":
	unittest.main()
