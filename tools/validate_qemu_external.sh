#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
# QEMU is an external dependency. This script never stores QEMU source in AyeDrive.
set -euo pipefail

expected_version="${QEMU_VERSION:?Set QEMU_VERSION to the pinned QEMU version}"
qemu_source="${QEMU_SOURCE_DIR:-}"
qemu_url="${QEMU_SOURCE_URL:-https://gitlab.com/qemu-project/qemu.git}"
qemu_checkout="${QEMU_CHECKOUT_DIR:-${TMPDIR:-/tmp}/ayeadrive-qemu-${expected_version}}"
qemu_build="${QEMU_BUILD_DIR:-${TMPDIR:-/tmp}/ayeadrive-qemu-build-${expected_version}}"

case "$qemu_checkout" in
  "$PWD"|"$PWD"/*)
	 echo "QEMU_CHECKOUT_DIR must be outside the AyeDrive repository" >&2
	 exit 2
;;
esac

if [ -z "$qemu_source" ]; then
	if [ -e "$qemu_checkout" ]; then
		qemu_source="$qemu_checkout"
	else
		git clone --branch "$expected_version" --depth 1 "$qemu_url" "$qemu_checkout"
		qemu_source="$qemu_checkout"
	fi
fi

if [ ! -d "$qemu_source/.git" ]; then
	echo "QEMU_SOURCE_DIR is not an official checkout: $qemu_source" >&2
	exit 2
fi
actual_version="$(git -C "$qemu_source" describe --tags --always --dirty)"
if [ "$actual_version" != "$expected_version" ]; then
	echo "QEMU version mismatch: expected $expected_version, got $actual_version" >&2
	exit 3
fi

if ! command -v meson >/dev/null; then
	echo "meson is required on the native host" >&2
	exit 2
fi
if ! command -v ninja >/dev/null; then
	echo "ninja is required on the native host" >&2
	exit 2
fi
meson setup "$qemu_build" "$qemu_source" --wipe
meson compile -C "$qemu_build"
"$qemu_build/qemu-system-x86_64" --version
printf 'AYEADRIVE_QEMU_EXTERNAL_VALIDATED version=%s build=%s\n' "$actual_version" "$qemu_build"
