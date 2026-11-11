#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
"""Manifest-driven Windows driver smoke-test runner."""

from __future__ import annotations

import argparse
import ctypes
import datetime as dt
import json
import os
import sys
import uuid
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any

TRANSPORTS = {
	"root", "usb", "bluetooth", "wifi", "pci", "storage", "network",
	"robot", "accelerator", "custom",
}


@dataclass
class TestResult:
	name: str
	status: str
	message: str
	details: dict[str, Any] | None = None


def load_manifest(path: Path) -> dict[str, Any]:
	with path.open("r", encoding="utf-8") as manifest_file:
		model = json.load(manifest_file)
	validate_manifest(model)
	return model


def validate_manifest(model: Any) -> None:
	if not isinstance(model, dict):
		raise ValueError("The manifest root must be a JSON object.")
	if model.get("schemaVersion") != 1:
		raise ValueError("schemaVersion must be 1.")
	if not isinstance(model.get("name"), str) or not model["name"].strip():
		raise ValueError("name must be a non-empty string.")
	driver = model.get("driver")
	if not isinstance(driver, dict) or not isinstance(driver.get("framework"), str) or not driver["framework"].strip():
		raise ValueError("driver.framework must be a non-empty string.")
	device = model.get("device")
	if not isinstance(device, dict) or device.get("transport") not in TRANSPORTS:
		raise ValueError(f"device.transport must be one of: {', '.join(sorted(TRANSPORTS))}.")
	if model.get("adapter") != "windows-interface":
		raise ValueError("This runner currently supports adapter 'windows-interface'.")
	try:
		uuid.UUID(str(device["interfaceGuid"]).strip("{}"))
	except (KeyError, ValueError, AttributeError) as error:
		raise ValueError("device.interfaceGuid must be a valid GUID for the selected Windows interface adapter.") from error
	access = device.get("access", ["read", "write"])
	if not isinstance(access, list) or not access or any(value not in {"read", "write"} for value in access):
		raise ValueError("device.access must be a non-empty array containing only read and/or write.")
	tests = model.get("tests", [])
	if not isinstance(tests, list):
		raise ValueError("tests must be a JSON array.")
	for index, test in enumerate(tests):
		if not isinstance(test, dict) or test.get("kind") not in {"ioctl", "read", "write"}:
			raise ValueError(f"tests[{index}].kind must be ioctl, read, or write.")
		if test["kind"] == "ioctl":
			parse_int(test.get("code"), f"tests[{index}].code")
			parse_hex(test.get("inputHex", ""), f"tests[{index}].inputHex")
			if not isinstance(test.get("outputLength", 0), int) or test.get("outputLength", 0) < 0:
				raise ValueError(f"tests[{index}].outputLength must be a non-negative integer.")
		elif test["kind"] == "read":
			if not isinstance(test.get("length"), int) or test["length"] <= 0:
				raise ValueError(f"tests[{index}].length must be a positive integer.")
		else:
			parse_hex(test.get("dataHex", ""), f"tests[{index}].dataHex")
			if not test.get("dataHex"):
				raise ValueError(f"tests[{index}].dataHex must not be empty.")


def parse_int(value: Any, label: str) -> int:
	try:
		return int(value, 0) if isinstance(value, str) else int(value)
	except (TypeError, ValueError) as error:
		raise ValueError(f"{label} must be an integer or 0x-prefixed integer string.") from error


def parse_hex(value: Any, label: str) -> bytes:
	if not isinstance(value, str):
		raise ValueError(f"{label} must be a hexadecimal string.")
	try:
		return bytes.fromhex(value)
	except ValueError as error:
		raise ValueError(f"{label} must contain valid hexadecimal bytes.") from error


def timestamp() -> str:
	return dt.datetime.now(dt.timezone.utc).isoformat()


def write_report(path: Path | None, model_name: str, results: list[TestResult], started_at: str) -> dict[str, Any]:
	report = {
		"schemaVersion": 1,
		"model": model_name,
		"startedAtUtc": started_at,
		"completedAtUtc": timestamp(),
		"status": "passed" if results and all(result.status == "passed" for result in results) else "failed",
		"results": [asdict(result) for result in results],
	}
	if path:
		path.parent.mkdir(parents=True, exist_ok=True)
		path.write_text(json.dumps(report, indent=2) + os.linesep, encoding="utf-8")
	print(json.dumps(report, indent=2))
	return report


class WindowsGuid(ctypes.Structure):
	_fields_ = [
		("Data1", ctypes.c_uint32),
		("Data2", ctypes.c_uint16),
		("Data3", ctypes.c_uint16),
		("Data4", ctypes.c_ubyte * 8),
	]


class DeviceInterfaceData(ctypes.Structure):
	_fields_ = [
		("cbSize", ctypes.c_uint32),
		("InterfaceClassGuid", WindowsGuid),
		("Flags", ctypes.c_uint32),
		("Reserved", ctypes.c_size_t),
	]


class WindowsInterfaceAdapter:
	GUID = WindowsGuid
	DeviceInterfaceData = DeviceInterfaceData
	"""Find a device interface, open it, and execute manifest I/O probes."""

	DIGCF_PRESENT = 0x00000002
	DIGCF_DEVICEINTERFACE = 0x00000010
	GENERIC_READ = 0x80000000
	GENERIC_WRITE = 0x40000000
	FILE_SHARE_READ = 0x00000001
	FILE_SHARE_WRITE = 0x00000002
	OPEN_EXISTING = 3
	INVALID_HANDLE_VALUE = ctypes.c_void_p(-1).value

	@classmethod
	def _guid(cls, value: str) -> GUID:
		parsed = uuid.UUID(value.strip("{}"))
		return cls.GUID(
			parsed.time_low,
			parsed.time_mid,
			parsed.time_hi_version,
			(ctypes.c_ubyte * 8).from_buffer_copy(parsed.bytes[8:]),
		)

	@classmethod
	def find_interfaces(cls, value: str) -> list[str]:
		if os.name != "nt":
			raise OSError("Device I/O tests must run inside the Windows guest.")

		setupapi = ctypes.WinDLL("setupapi", use_last_error=True)
		setupapi.SetupDiGetClassDevsW.argtypes = [ctypes.POINTER(cls.GUID), ctypes.c_wchar_p, ctypes.c_void_p, ctypes.c_uint32]
		setupapi.SetupDiGetClassDevsW.restype = ctypes.c_void_p
		setupapi.SetupDiEnumDeviceInterfaces.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.POINTER(cls.GUID), ctypes.c_uint32, ctypes.POINTER(cls.DeviceInterfaceData)]
		setupapi.SetupDiEnumDeviceInterfaces.restype = ctypes.c_int
		setupapi.SetupDiGetDeviceInterfaceDetailW.argtypes = [ctypes.c_void_p, ctypes.POINTER(cls.DeviceInterfaceData), ctypes.c_void_p, ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32), ctypes.c_void_p]
		setupapi.SetupDiGetDeviceInterfaceDetailW.restype = ctypes.c_int
		setupapi.SetupDiDestroyDeviceInfoList.argtypes = [ctypes.c_void_p]
		setupapi.SetupDiDestroyDeviceInfoList.restype = ctypes.c_int

		guid = cls._guid(value)
		device_set = setupapi.SetupDiGetClassDevsW(ctypes.byref(guid), None, None, cls.DIGCF_PRESENT | cls.DIGCF_DEVICEINTERFACE)
		if device_set == cls.INVALID_HANDLE_VALUE:
			raise ctypes.WinError(ctypes.get_last_error())

		paths: list[str] = []
		try:
			index = 0
			while True:
				interface = cls.DeviceInterfaceData()
				interface.cbSize = ctypes.sizeof(interface)
				if not setupapi.SetupDiEnumDeviceInterfaces(device_set, None, ctypes.byref(guid), index, ctypes.byref(interface)):
					error = ctypes.get_last_error()
					if error == 259:  # ERROR_NO_MORE_ITEMS
						break
					raise ctypes.WinError(error)

				required = ctypes.c_uint32()
				setupapi.SetupDiGetDeviceInterfaceDetailW(device_set, ctypes.byref(interface), None, 0, ctypes.byref(required), None)
				detail = ctypes.create_string_buffer(required.value)
				ctypes.cast(detail, ctypes.POINTER(ctypes.c_uint32)).contents.value = 8 if ctypes.sizeof(ctypes.c_void_p) == 8 else 6
				if not setupapi.SetupDiGetDeviceInterfaceDetailW(device_set, ctypes.byref(interface), detail, required.value, ctypes.byref(required), None):
					raise ctypes.WinError(ctypes.get_last_error())
				paths.append(ctypes.wstring_at(ctypes.addressof(detail) + ctypes.sizeof(ctypes.c_uint32)))
				index += 1
		finally:
			setupapi.SetupDiDestroyDeviceInfoList(device_set)
		return paths

	@classmethod
	def open_device(cls, path: str, access: list[str]) -> int:
		kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
		kernel32.CreateFileW.argtypes = [ctypes.c_wchar_p, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_void_p]
		kernel32.CreateFileW.restype = ctypes.c_void_p
		desired_access = 0
		if "read" in access:
			desired_access |= cls.GENERIC_READ
		if "write" in access:
			desired_access |= cls.GENERIC_WRITE
		handle = kernel32.CreateFileW(path, desired_access, cls.FILE_SHARE_READ | cls.FILE_SHARE_WRITE, None, cls.OPEN_EXISTING, 0, None)
		if handle == cls.INVALID_HANDLE_VALUE:
			raise ctypes.WinError(ctypes.get_last_error())
		return handle

	@classmethod
	def execute(cls, handle: int, test: dict[str, Any], allow_write: bool) -> TestResult:
		kind = test["kind"]
		name = test.get("name", kind)
		kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
		transferred = ctypes.c_uint32()

		if kind == "ioctl":
			input_data = parse_hex(test.get("inputHex", ""), "inputHex")
			output_length = test.get("outputLength", 0)
			input_buffer = ctypes.create_string_buffer(input_data, len(input_data)) if input_data else None
			output_buffer = ctypes.create_string_buffer(output_length) if output_length else None
			kernel32.DeviceIoControl.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_void_p, ctypes.c_uint32, ctypes.c_void_p, ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32), ctypes.c_void_p]
			kernel32.DeviceIoControl.restype = ctypes.c_int
			succeeded = bool(kernel32.DeviceIoControl(handle, parse_int(test["code"], "code"), input_buffer, len(input_data), output_buffer, output_length, ctypes.byref(transferred), None))
			output_data = output_buffer.raw[:transferred.value] if output_buffer else b""
		elif kind == "read":
			output_length = test["length"]
			output_buffer = ctypes.create_string_buffer(output_length)
			kernel32.ReadFile.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32), ctypes.c_void_p]
			kernel32.ReadFile.restype = ctypes.c_int
			succeeded = bool(kernel32.ReadFile(handle, output_buffer, output_length, ctypes.byref(transferred), None))
			output_data = output_buffer.raw[:transferred.value]
		else:
			if not allow_write:
				return TestResult(name, "blocked", "Write probes require the explicit --allow-write option.")
			input_data = parse_hex(test["dataHex"], "dataHex")
			input_buffer = ctypes.create_string_buffer(input_data, len(input_data))
			kernel32.WriteFile.argtypes = [ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32), ctypes.c_void_p]
			kernel32.WriteFile.restype = ctypes.c_int
			succeeded = bool(kernel32.WriteFile(handle, input_buffer, len(input_data), ctypes.byref(transferred), None))
			output_data = b""

		error = 0 if succeeded else ctypes.get_last_error()
		expected_success = bool(test.get("expectSuccess", True))
		passed = succeeded == expected_success
		expected_error = test.get("expectWin32Error")
		if expected_error is not None and error != int(expected_error):
			passed = False
		expected_output = test.get("expectOutputHex")
		if succeeded and expected_output is not None and output_data != parse_hex(expected_output, "expectOutputHex"):
			passed = False
		expected_bytes = test.get("expectBytesTransferred")
		if expected_bytes is not None and transferred.value != int(expected_bytes):
			passed = False
		details = {"success": succeeded, "win32Error": error, "bytesTransferred": transferred.value}
		if kind != "write":
			details["outputHex"] = output_data.hex()
		return TestResult(name, "passed" if passed else "failed", "I/O result matched the manifest." if passed else "I/O result did not match the manifest.", details)

	@classmethod
	def run(cls, model: dict[str, Any], allow_write: bool) -> list[TestResult]:
		paths = cls.find_interfaces(model["device"]["interfaceGuid"])
		if not paths:
			return [TestResult("device-interface-discovery", "failed", "No present device interface matched the configured GUID.")]
		results = [TestResult("device-interface-discovery", "passed", f"Found {len(paths)} matching interface(s).", {"paths": paths})]
		try:
			handle = cls.open_device(paths[0], model["device"].get("access", ["read", "write"]))
		except OSError as error:
			results.append(TestResult("device-open", "failed", str(error)))
			return results
		results.append(TestResult("device-open", "passed", "Opened the first matching device interface."))
		kernel32 = ctypes.WinDLL("kernel32", use_last_error=True)
		kernel32.CloseHandle.argtypes = [ctypes.c_void_p]
		kernel32.CloseHandle.restype = ctypes.c_int
		try:
			for test in model.get("tests", []):
				results.append(cls.execute(handle, test, allow_write))
		finally:
			kernel32.CloseHandle(handle)
		return results


def main(argv: list[str] | None = None) -> int:
	parser = argparse.ArgumentParser(description="Manifest-driven Windows driver test harness")
	commands = parser.add_subparsers(dest="command", required=True)
	validate = commands.add_parser("validate", help="validate a device-model manifest")
	validate.add_argument("manifest", type=Path)
	test = commands.add_parser("test", help="discover and exercise a Windows device interface")
	test.add_argument("manifest", type=Path)
	test.add_argument("--report", type=Path, help="write a JSON test report")
	test.add_argument("--allow-write", action="store_true", help="allow manifest write probes")
	commands.add_parser("list-transports", help="list the transport types recognized by manifests")
	args = parser.parse_args(argv)

	if args.command == "list-transports":
		print("\n".join(sorted(TRANSPORTS)))
		return 0
	try:
		model = load_manifest(args.manifest)
		if args.command == "validate":
			print(f"Valid model: {model['name']}")
			return 0
		started_at = timestamp()
		results = WindowsInterfaceAdapter.run(model, args.allow_write)
		report = write_report(args.report, model["name"], results, started_at)
		return 0 if report["status"] == "passed" else 1
	except (OSError, ValueError, json.JSONDecodeError) as error:
		print(f"driver-test: {error}", file=sys.stderr)
		return 2


if __name__ == "__main__":
	raise SystemExit(main())
