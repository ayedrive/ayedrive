#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Render guest HAL framebuffer frames received from a QEMU virtio-serial socket."""
import argparse
import socket
import struct
import sys
import time
import tkinter

MAGIC = 0x44504344
VERSION = 1
HEADER = struct.Struct("<6I")

def receive(sock, size):
	data = bytearray()
	while len(data) < size:
		chunk = sock.recv(size - len(data))
		if not chunk:
			raise EOFError
		data.extend(chunk)
	return bytes(data)

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("socket")
	args = parser.parse_args()
	connection = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
	for _ in range(100):
		try:
			connection.connect(args.socket)
			break
		except FileNotFoundError:
			time.sleep(0.1)
	else:
		raise RuntimeError("QEMU display socket was not created: %s" % args.socket)
	root = tkinter.Tk()
	root.title("AyeDrive QEMU HAL Display")
	canvas = tkinter.Canvas(root, width=640, height=480)
	canvas.pack()
	image = tkinter.PhotoImage(width=640, height=480)
	canvas.create_image(0, 0, image=image, anchor=tkinter.NW)
	connection.setblocking(False)
	buffer = bytearray()
	def poll():
		try:
			buffer.extend(connection.recv(65536))
		except BlockingIOError:
			pass
		while len(buffer) >= HEADER.size:
			magic, version, width, height, payload_length, _ = HEADER.unpack(buffer[:HEADER.size])
			if magic != MAGIC or version != VERSION or width == 0 or height == 0 or payload_length != width * height * 4:
				raise RuntimeError("invalid display transport frame")
			if len(buffer) < HEADER.size + payload_length:
				break
			payload = bytes(buffer[HEADER.size:HEADER.size + payload_length])
			del buffer[:HEADER.size + payload_length]
			rows = []
			for offset in range(0, len(payload), width * 4):
				row = payload[offset:offset + width * 4]
				rows.append("{" + " ".join("#%02x%02x%02x" % tuple(row[i:i + 3]) for i in range(0, len(row), 4)) + "}")
			image.put(" ".join(rows))
		root.after(16, poll)
	root.after(16, poll)
	root.mainloop()
	return 0

if __name__ == "__main__":
	sys.exit(main())
