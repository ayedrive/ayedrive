#!/usr/bin/env python3
"""Serve the visible AyeDrive WebGPU display viewer."""
from __future__ import annotations

import argparse
import http.server
import struct
import subprocess
import threading
import webbrowser
from pathlib import Path


def main() -> int:
	parser = argparse.ArgumentParser()
	parser.add_argument("--host", default="127.0.0.1")
	parser.add_argument("--port", type=int, default=8765)
	parser.add_argument("--no-browser", action="store_true")
	parser.add_argument("--producer", type=Path)
	args = parser.parse_args()
	root = Path(__file__).with_name("webgpu_display")
	producer_path = args.producer or Path(__file__).parent.parent / "Shared" / "build" / "display_web_source"
	producer = None
	producer_lock = threading.Lock()
	if producer_path.is_file():
		producer = subprocess.Popen(
			[str(producer_path)],
			stdout=subprocess.PIPE,
			stderr=subprocess.DEVNULL,
		)

	class Handler(http.server.SimpleHTTPRequestHandler):
		def __init__(self, *handler_args, **handler_kwargs):
			super().__init__(*handler_args, directory=str(root), **handler_kwargs)

		def do_GET(self):
			if self.path == "/frame":
				if producer is None or producer.stdout is None:
					self.send_error(503, "C display HAL producer is not available")
					return
				with producer_lock:
					header = producer.stdout.read(24)
					if len(header) != 24:
						self.send_error(503, "C display HAL producer stopped")
						return
					magic, version, frame_width, frame_height, payload_length, _reserved = struct.unpack("<6I", header)
					expected_length = frame_width * frame_height * 4
					if (
						magic != 0x44504344
						or version != 1
						or frame_width != 640
						or frame_height != 480
						or payload_length != expected_length
					):
						self.send_error(502, "invalid C display HAL frame")
						return
					payload = producer.stdout.read(payload_length)
				if len(payload) != payload_length:
					self.send_error(503, "short C display HAL frame")
					return
				self.send_response(200)
				self.send_header("Content-Type", "application/octet-stream")
				self.send_header("Content-Length", str(payload_length))
				self.end_headers()
				self.wfile.write(payload)
				return
			super().do_GET()

	server = http.server.ThreadingHTTPServer((args.host, args.port), Handler)
	url = f"http://{args.host}:{args.port}/index.html"
	print(f"AyeDrive WebGPU display: {url}")
	print("This viewer requires framebuffer bytes from the C HAL /frame endpoint.")
	if not args.no_browser:
		threading.Timer(0.25, lambda: webbrowser.open(url)).start()
	try:
		server.serve_forever()
	except KeyboardInterrupt:
		pass
	finally:
		server.server_close()
		if producer is not None:
			producer.terminate()
	return 0


if __name__ == "__main__":
	raise SystemExit(main())
