# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import tempfile
import unittest
from pathlib import Path

from driver_factory.site import build


class StaticSiteTests(unittest.TestCase):
	def test_build_executes_all_notebooks_and_writes_pages(self) -> None:
		with tempfile.TemporaryDirectory() as temp_dir:
			output = Path(temp_dir) / "site"
			written = build(output)
			training_pages = sorted((output / "training").glob("*.html"))
			self.assertEqual(9, len(training_pages))
			self.assertTrue((output / "index.html").is_file())
			self.assertTrue((output / "training" / "index.html").is_file())
			self.assertGreaterEqual(len(written), 10)
			self.assertIn("not product approval", (output / "index.html").read_text(encoding="utf-8"))
			lesson = (output / "training" / "01-interface-impedance.html").read_text(encoding="utf-8")
			self.assertIn('href="../index.html"', lesson)
			self.assertIn('href="index.html"', lesson)


if __name__ == "__main__":
	unittest.main()
