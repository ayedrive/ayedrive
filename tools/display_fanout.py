#!/usr/bin/env python3
"""Fan out one validated QEMU guest framebuffer stream to browser and X11 sinks."""
from __future__ import annotations

import argparse
import http.server
import socket
import struct
import threading
import tkinter
from pathlib import Path

MAGIC = 0x44504344
HEADER = struct.Struct("<6I")
MAX_WIDTH = 4096
MAX_HEIGHT = 2160
MAX_FRAME = MAX_WIDTH * MAX_HEIGHT * 4

class State:
	def __init__(self):
		self.condition = threading.Condition()
		self.frame = None
		self.number = -1
		self.width = 0
		self.height = 0
		self.error = "waiting for QEMU guest frame stream"
		self.text = ""


def read_exact(stream, size):
	data = bytearray()
	while len(data) < size:
		chunk = stream.recv(size - len(data))
		if not chunk:
			raise EOFError
		data.extend(chunk)
	return bytes(data)


def consume(connection, state):
	try:
		while True:
			header = read_exact(connection, HEADER.size)
			magic, version, width, height, payload_length, number = HEADER.unpack(header)
			if (magic != MAGIC or version != 1 or width == 0 or height == 0 or
					width > MAX_WIDTH or height > MAX_HEIGHT or
					payload_length != width * height * 4 or payload_length > MAX_FRAME):
				raise RuntimeError("invalid QEMU guest display frame")
			payload = read_exact(connection, payload_length)
			with state.condition:
				state.frame = payload
				state.number = number
				state.width = width
				state.height = height
				state.error = ""
				state.condition.notify_all()
			if number == 0:
				print("DPC_FANOUT_FIRST_FRAME", flush=True)
	except (EOFError, OSError, RuntimeError) as error:
		with state.condition:
			state.error = str(error)
			state.condition.notify_all()
		print("DPC_FANOUT_ERROR=%s" % state.error, flush=True)


def consume_serial(connection, state):
	try:
		while True:
			data = connection.recv(4096)
			if not data:
				raise EOFError
			text = data.decode("utf-8", errors="replace").replace("\x1b", "")
			with state.condition:
				state.text = (state.text + text)[-12000:]
				state.condition.notify_all()
			print("DPC_FANOUT_SERIAL_BYTES=%d" % len(data), flush=True)
	except (EOFError, OSError) as error:
		with state.condition:
			state.error = "serial: %s" % error
			state.condition.notify_all()


def accept_serial_loop(server, state):
	while True:
		connection, _ = server.accept()
		print("DPC_FANOUT_SERIAL_CONNECTED", flush=True)
		consume_serial(connection, state)


def accept_loop(server, state):
	while True:
		connection, _ = server.accept()
		print("DPC_FANOUT_GUEST_CONNECTED", flush=True)
		consume(connection, state)


def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("--socket", required=True, help="QEMU Unix socket or TCP host:port")
	parser.add_argument("--http-port", type=int, default=8765)
	parser.add_argument("--x11", action="store_true")
	parser.add_argument("--serial-socket", help="QEMU ttyS0 Unix socket")
	parser.add_argument("--mode", choices=("framebuffer", "text"), default="framebuffer")
	args = parser.parse_args()
	state = State()
	if ":" in args.socket and not args.socket.startswith("/"):
		host, port = args.socket.rsplit(":", 1)
		server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		server.bind((host, int(port)))
		server.listen(1)
		listener = server
	else:
		server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
		server.bind(args.socket)
		server.listen(1)
		listener = server
	threading.Thread(target=accept_loop, args=(listener, state), daemon=True).start()
	if args.serial_socket:
		serial_server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
		serial_server.bind(args.serial_socket)
		serial_server.listen(1)
		threading.Thread(target=accept_serial_loop, args=(serial_server, state), daemon=True).start()
	root = Path(__file__).with_name("webgpu_display")

	class Handler(http.server.SimpleHTTPRequestHandler):
		def __init__(self, *items, **kwargs):
			super().__init__(*items, directory=str(root), **kwargs)
		def do_GET(self):
			if self.path == "/console":
				with state.condition:
					payload = state.text
				self.send_response(200)
				self.send_header("Content-Type", "text/plain; charset=utf-8")
				self.send_header("Cache-Control", "no-store")
				self.end_headers()
				self.wfile.write(payload.encode("utf-8", errors="replace"))
				return
			if self.path == "/frame":
				with state.condition:
					if state.frame is None:
						self.send_error(503, state.error)
						return
					payload = state.frame
					frame_width = state.width
					frame_height = state.height
				self.send_response(200)
				self.send_header("Content-Type", "application/octet-stream")
				self.send_header("Content-Length", str(len(payload)))
				self.send_header("X-AyeDrive-Width", str(frame_width))
				self.send_header("X-AyeDrive-Height", str(frame_height))
				self.end_headers()
				self.wfile.write(payload)
				return
			super().do_GET()

	if args.x11:
		ui = tkinter.Tk()
		ui.title("AyeDrive QEMU guest HAL display")
		if args.mode == "text":
			text = tkinter.Text(ui, width=100, height=32, bg="#0c1220", fg="#b9f6ca", insertbackground="#b9f6ca")
			text.pack(fill="both", expand=True)
			def paint_text():
				with state.condition:
					value = state.text
				text.delete("1.0", tkinter.END)
				text.insert("1.0", value)
				ui.after(50, paint_text)
			ui.after(50, paint_text)
			threading.Thread(target=lambda: http.server.ThreadingHTTPServer(("0.0.0.0", args.http_port), Handler).serve_forever(), daemon=True).start()
			ui.mainloop()
			return
		image = None
		canvas = tkinter.Canvas(ui)
		canvas.pack(fill="both", expand=True)
		canvas_image = [None]
		def paint():
			nonlocal image
			with state.condition:
				payload = state.frame
				frame_width = state.width
				frame_height = state.height
			if payload is not None and (image is None or image.width() != frame_width or image.height() != frame_height):
				image = tkinter.PhotoImage(width=frame_width, height=frame_height)
				canvas.delete("all")
				canvas_image[0] = canvas.create_image(0, 0, image=image, anchor=tkinter.NW)
				canvas.config(scrollregion=(0, 0, frame_width, frame_height))
			if payload is not None and image is not None:
				rows = []
				for offset in range(0, len(payload), frame_width * 4):
					row = payload[offset:offset + frame_width * 4]
					rows.append("{" + " ".join("#%02x%02x%02x" % tuple(row[i:i + 3]) for i in range(0, len(row), 4)) + "}")
				image.put(" ".join(rows))
			ui.after(16, paint)
		ui.after(16, paint)
		threading.Thread(target=lambda: http.server.ThreadingHTTPServer(("0.0.0.0", args.http_port), Handler).serve_forever(), daemon=True).start()
		ui.mainloop()
	else:
		http.server.ThreadingHTTPServer(("0.0.0.0", args.http_port), Handler).serve_forever()

if __name__ == "__main__":
	main()
