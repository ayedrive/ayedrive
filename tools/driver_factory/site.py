# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

from __future__ import annotations

import html
import json
import re
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
NOTEBOOKS = ROOT / "notebooks"
DEFAULT_OUTPUT = ROOT / "docs" / "_site"


def _inline(text: str) -> str:
	escaped = html.escape(text, quote=True)
	escaped = re.sub(r"`([^`]+)`", r"<code>\1</code>", escaped)
	escaped = re.sub(r"\[([^\]]+)\]\(([^)]+)\)", r'<a href="\2">\1</a>', escaped)
	return escaped


def _render_markdown(source: str) -> str:
	lines = source.splitlines()
	output: list[str] = []
	in_code = False
	code_lines: list[str] = []
	paragraph: list[str] = []

	def flush_paragraph() -> None:
		if paragraph:
			output.append("<p>" + " ".join(_inline(line.strip()) for line in paragraph) + "</p>")
			paragraph.clear()

	for line in lines:
		if line.startswith("```"):
			flush_paragraph()
			if in_code:
				output.append("<pre><code>" + html.escape("\n".join(code_lines)) + "</code></pre>")
				code_lines.clear()
			in_code = not in_code
		elif in_code:
			code_lines.append(line)
		elif not line.strip():
			flush_paragraph()
		elif line.startswith("#"):
			flush_paragraph()
			level = min(len(line) - len(line.lstrip("#")), 6)
			title = line[level:].strip()
			output.append(f"<h{level}>{_inline(title)}</h{level}>")
		elif line.startswith("- "):
			flush_paragraph()
			output.append("<li>" + _inline(line[2:]) + "</li>")
		elif line.startswith("|"):
			flush_paragraph()
			output.append("<pre class=\"table\">" + html.escape(line) + "</pre>")
		else:
			paragraph.append(line)
	flush_paragraph()
	if in_code:
		output.append("<pre><code>" + html.escape("\n".join(code_lines)) + "</code></pre>")
	return "\n".join(output)


def _page(title: str, body: str, home_href: str = "index.html", training_href: str = "training/index.html") -> str:
	template = """<!doctype html>
<html lang="en"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>__TITLE__ — Driver Factory</title>
<style>body{font:16px/1.6 system-ui,sans-serif;max-width:980px;margin:2rem auto;padding:0 1rem;color:#17212b}nav{background:#eef3f8;padding:.75rem}code,pre{background:#f4f6f8}pre{padding:1rem;overflow:auto}li{margin:.35rem 0}.notice{border-left:4px solid #b45309;padding:.5rem 1rem;background:#fff7ed}a{color:#075985}</style></head>
<body><nav><a href="__HOME__">Home</a> · <a href="__TRAINING__">Training</a></nav>
<div class="notice"><strong>Reference platform only.</strong> Generated pages, models, and tests are not product approval, device compatibility, HIL evidence, or certification.</div>
__BODY__
</body></html>"""
	return (template.replace("__TITLE__", html.escape(title))
		.replace("__HOME__", html.escape(home_href, quote=True))
		.replace("__TRAINING__", html.escape(training_href, quote=True))
		.replace("__BODY__", body))


def _execute_notebook(path: Path) -> tuple[str, list[dict[str, Any]]]:
	notebook = json.loads(path.read_text(encoding="utf-8"))
	namespace: dict[str, Any] = {"__name__": "__notebook__"}
	rendered: list[dict[str, Any]] = []
	for index, cell in enumerate(notebook.get("cells", [])):
		source = "".join(cell.get("source", []))
		if cell.get("cell_type") == "markdown":
			rendered.append({"type": "markdown", "html": _render_markdown(source)})
		elif cell.get("cell_type") == "code":
			exec(compile(source, f"{path.name}:cell-{index}", "exec"), namespace, namespace)
			rendered.append({"type": "code", "source": source})
	return notebook.get("metadata", {}).get("title", path.stem), rendered


def build(output: Path = DEFAULT_OUTPUT) -> list[str]:
	output.mkdir(parents=True, exist_ok=True)
	written: list[str] = []
	docs = [ROOT / "docs" / "index.md", ROOT / "docs" / "architecture.md", ROOT / "docs" / "target-matrix.md", ROOT / "docs" / "device-dossier-automation.md", ROOT / "docs" / "qemu-ephemeral-testing.md", ROOT / "docs" / "static-site.md", ROOT / "docs" / "critical-systems-validation.md", ROOT / "docs" / "adapter-contract.md", ROOT / "docs" / "assurance-automation.md", ROOT / "docs" / "ayeadrive-simulation-howto.md", ROOT / "docs" / "ayeadrive-release-status.md", ROOT / "docs" / "certification-evidence-status.md", ROOT / "docs" / "development-history.md", ROOT / "docs" / "safety" / "README.md", ROOT / "docs" / "training" / "07-synthetic-virtual-devices.md", ROOT / "docs" / "training" / "08-ayeadrive-independent-simulation.md"]
	links: list[str] = []
	for doc in docs:
		if doc.is_file():
			relative = doc.relative_to(ROOT).as_posix()
			if doc == ROOT / "docs" / "index.md":
				name = "index.html"
			else:
				name = relative.removeprefix("docs/").replace("/", "_").replace(".md", ".html")
			target = output / name
			target.write_text(_page(doc.stem, _render_markdown(doc.read_text(encoding="utf-8"))), encoding="utf-8")
			written.append(str(target))
			links.append(f'<li><a href="{html.escape(target.name)}">{html.escape(doc.stem)}</a></li>')
	training_dir = output / "training"
	training_dir.mkdir(exist_ok=True)
	training_links: list[str] = []
	for notebook_path in sorted(NOTEBOOKS.glob("*.ipynb")):
		title, cells = _execute_notebook(notebook_path)
		cell_html = []
		for cell in cells:
			if cell["type"] == "markdown":
				cell_html.append(cell["html"])
			else:
				cell_html.append("<pre><code>" + html.escape(cell["source"]) + "</code></pre>")
		target = training_dir / (notebook_path.stem + ".html")
		target.write_text(_page(title, "\n".join(cell_html), "../index.html", "index.html"), encoding="utf-8")
		written.append(str(target))
		training_links.append(f'<li><a href="{html.escape(target.name)}">{html.escape(title)}</a></li>')
	(training_dir / "index.html").write_text(_page("Training notebooks", "<h1>Training notebooks</h1><ul>" + "".join(training_links) + "</ul>"), encoding="utf-8")
	written.append(str(training_dir / "index.html"))
	return written


def main(argv: list[str] | None = None) -> int:
	output = Path(argv[0]) if argv else DEFAULT_OUTPUT
	try:
		files = build(output)
	except (OSError, ValueError, json.JSONDecodeError, SyntaxError) as error:
		print(f"SITE_BUILD_ERROR: {error}", file=sys.stderr)
		return 2
	print(f"SITE_BUILD_OK files={len(files)} output={output}")
	return 0


if __name__ == "__main__":
	raise SystemExit(main(sys.argv[1:]))
