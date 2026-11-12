# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from driver_factory.provenance import build_manifest, verify_manifest


class ProvenanceTests(unittest.TestCase):
	def test_hash_inventory_verifies_and_detects_tampering(self) -> None:
		with tempfile.TemporaryDirectory() as temp_dir:
			root = Path(temp_dir)
			artifact = root / "artifact.txt"
			artifact.write_text("original", encoding="utf-8")
			manifest = build_manifest(root, [artifact])
			verify_manifest(root, manifest)
			artifact.write_text("changed", encoding="utf-8")
			with self.assertRaisesRegex(ValueError, "hash mismatch"):
				verify_manifest(root, manifest)

	def test_path_traversal_is_rejected(self) -> None:
		with tempfile.TemporaryDirectory() as temp_dir:
			root = Path(temp_dir)
			manifest = {"formatVersion": 1, "algorithm": "sha256-file-inventory", "files": [{"path": "../outside", "sha256": "0", "bytes": 0}]}
			with self.assertRaisesRegex(ValueError, "unsafe"):
				verify_manifest(root, manifest)

	def test_empty_manifest_is_rejected(self) -> None:
		with tempfile.TemporaryDirectory() as temp_dir:
			manifest = {"formatVersion": 1, "algorithm": "sha256-file-inventory", "files": []}
			with self.assertRaisesRegex(ValueError, "no files"):
				verify_manifest(Path(temp_dir), manifest)

	def test_incomplete_inventory_is_rejected(self) -> None:
		with tempfile.TemporaryDirectory() as temp_dir:
			root = Path(temp_dir)
			(root / "one.txt").write_text("one", encoding="utf-8")
			(root / "two.txt").write_text("two", encoding="utf-8")
			complete = build_manifest(root, [root / "one.txt"])
			with self.assertRaisesRegex(ValueError, "file set mismatch"):
				verify_manifest(root, complete)


if __name__ == "__main__":
	unittest.main()
