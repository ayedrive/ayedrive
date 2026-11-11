# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import driver_test


class ManifestTests(unittest.TestCase):
	def test_sample_manifest_is_valid(self):
		model_path = Path(__file__).resolve().parents[1] / "models" / "umdf2-root.json"
		model = driver_test.load_manifest(model_path)
		self.assertEqual("root", model["device"]["transport"])
		self.assertEqual("UMDF", model["driver"]["framework"])

	def test_transport_catalog_contains_requested_classes(self):
		self.assertTrue({"usb", "bluetooth", "wifi", "pci", "robot", "accelerator"}.issubset(driver_test.TRANSPORTS))

	def test_device_class_catalog_matches_recognized_transports(self):
		catalog_path = Path(__file__).resolve().parents[1] / "models" / "device-classes.json"
		import json

		catalog = json.loads(catalog_path.read_text(encoding="utf-8"))
		self.assertEqual(driver_test.TRANSPORTS, {profile["transport"] for profile in catalog["profiles"]})

	def test_framework_labels_are_extensible(self):
		model = driver_test.load_manifest(Path(__file__).resolve().parents[1] / "models" / "umdf2-root.json")
		model["driver"]["framework"] = "VendorSpecificMiniport"
		driver_test.validate_manifest(model)

	def test_invalid_guid_is_rejected(self):
		model = {
			"schemaVersion": 1,
			"name": "invalid",
			"driver": {"framework": "UMDF"},
			"device": {"transport": "root", "interfaceGuid": "not-a-guid"},
			"adapter": "windows-interface",
			"tests": [],
		}
		with self.assertRaisesRegex(ValueError, "interfaceGuid"):
			driver_test.validate_manifest(model)

	def test_write_test_requires_hex_data(self):
		model = {
			"schemaVersion": 1,
			"name": "invalid",
			"driver": {"framework": "UMDF"},
			"device": {"transport": "root", "interfaceGuid": "1495036f-aa8f-459f-bf25-f2cfdf85452c"},
			"adapter": "windows-interface",
			"tests": [{"kind": "write", "dataHex": ""}],
		}
		with self.assertRaisesRegex(ValueError, "must not be empty"):
			driver_test.validate_manifest(model)


if __name__ == "__main__":
	unittest.main()
