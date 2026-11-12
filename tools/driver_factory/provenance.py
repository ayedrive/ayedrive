# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any


def build_manifest(root: Path, paths: list[Path]) -> dict[str, Any]:
	resolved_root = root.resolve()
	records = []
	for path in sorted((candidate.resolve() for candidate in paths), key=lambda item: item.as_posix()):
		if resolved_root not in path.parents:
			raise ValueError(f"input is outside provenance root: {path}")
		if not path.is_file():
			raise ValueError(f"input is not a regular file: {path}")
		relative = path.relative_to(resolved_root).as_posix()
		digest = hashlib.sha256(path.read_bytes()).hexdigest()
		records.append({"path": relative, "sha256": digest, "bytes": path.stat().st_size})
	return {"formatVersion": 1, "algorithm": "sha256-file-inventory", "files": records, "signature": "not_signed"}


def verify_manifest(root: Path, manifest: dict[str, Any]) -> None:
	if manifest.get("formatVersion") != 1 or manifest.get("algorithm") != "sha256-file-inventory":
		raise ValueError("unsupported provenance manifest format")
	seen: set[str] = set()
	if not isinstance(manifest.get("files"), list):
		raise ValueError("provenance files must be an array")
	for record in manifest.get("files", []):
		relative = Path(record["path"])
		if relative.is_absolute() or ".." in relative.parts or relative.as_posix() in seen:
			raise ValueError(f"unsafe or duplicate provenance path: {relative}")
		seen.add(relative.as_posix())
		path = (root / relative).resolve()
		if root.resolve() not in path.parents or not path.is_file():
			raise ValueError(f"missing or escaping provenance file: {relative}")
		if hashlib.sha256(path.read_bytes()).hexdigest() != record["sha256"]:
			raise ValueError(f"hash mismatch: {relative}")
		if path.stat().st_size != record["bytes"]:
			raise ValueError(f"size mismatch: {relative}")
	if not seen:
		raise ValueError("provenance manifest contains no files")
	expected = {
		path.relative_to(root.resolve()).as_posix()
		for path in root.resolve().rglob("*")
		if path.is_file() and path.name != "provenance.manifest.json"
	}
	if seen != expected:
		missing = sorted(expected - seen)
		extra = sorted(seen - expected)
		raise ValueError(f"inventory file set mismatch; missing={missing}, extra={extra}")


def write_manifest(root: Path, files: list[Path], output: Path) -> None:
	manifest = build_manifest(root, files)
	output.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8")
