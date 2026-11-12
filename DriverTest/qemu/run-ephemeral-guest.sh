#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
set -euo pipefail

: "${GUEST_ISO:?Set GUEST_ISO to an authorized local installer/live ISO}"
: "${GUEST_ISO_SHA256:?Set GUEST_ISO_SHA256 to the independently obtained expected SHA-256}"

QEMU_SYSTEM="${QEMU_SYSTEM:-qemu-system-x86_64}"
QEMU_IMG="${QEMU_IMG:-qemu-img}"
QEMU_SERIAL_LOG="${QEMU_SERIAL_LOG:-}"
GUEST_RAM_MB="${GUEST_RAM_MB:-2048}"
GUEST_DISK_GB="${GUEST_DISK_GB:-12}"
MIN_FREE_MB="${MIN_FREE_MB:-4096}"
KEEP_GUEST_WORKDIR="${KEEP_GUEST_WORKDIR:-0}"
WORK_ROOT="${WORK_ROOT:-${TMPDIR:-/tmp}}"

fail() {
	printf 'ERROR: %s\n' "$1" >&2
	exit 2
}

command -v "$QEMU_SYSTEM" >/dev/null 2>&1 || fail "QEMU executable not found: $QEMU_SYSTEM"
command -v "$QEMU_IMG" >/dev/null 2>&1 || fail "qemu-img not found: $QEMU_IMG"
[[ -f "$GUEST_ISO" ]] || fail "GUEST_ISO is not a regular file"
[[ "$GUEST_ISO_SHA256" =~ ^[[:xdigit:]]{64}$ ]] || fail "GUEST_ISO_SHA256 must contain 64 hexadecimal characters"
[[ "$GUEST_RAM_MB" =~ ^[0-9]+$ ]] || fail "GUEST_RAM_MB must be an integer"
[[ "$GUEST_DISK_GB" =~ ^[0-9]+$ ]] || fail "GUEST_DISK_GB must be an integer"
[[ "$MIN_FREE_MB" =~ ^[0-9]+$ ]] || fail "MIN_FREE_MB must be an integer"
(( GUEST_RAM_MB >= 512 && GUEST_RAM_MB <= 32768 )) || fail "GUEST_RAM_MB must be between 512 and 32768"
(( GUEST_DISK_GB >= 4 && GUEST_DISK_GB <= 128 )) || fail "GUEST_DISK_GB must be between 4 and 128"
(( MIN_FREE_MB >= 1024 )) || fail "MIN_FREE_MB must be at least 1024"

actual_sha="$(sha256sum "$GUEST_ISO" | awk '{print $1}')"
[[ "${actual_sha,,}" == "${GUEST_ISO_SHA256,,}" ]] || fail "guest ISO SHA-256 does not match expected value"

mkdir -p "$WORK_ROOT"
work_dir="$(mktemp -d "$WORK_ROOT/driver-factory-qemu.XXXXXXXX")"
cleanup() {
	trap - EXIT INT TERM
	if [[ "$KEEP_GUEST_WORKDIR" == "1" ]]; then
		printf 'Guest artifacts retained at %s\n' "$work_dir"
	else
		rm -rf -- "$work_dir"
		printf 'Ephemeral guest artifacts deleted.\n'
	fi
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

available_kb="$(df -Pk "$work_dir" | awk 'NR==2 {print $4}')"
iso_bytes="$(stat -c '%s' "$GUEST_ISO")"
required_kb=$(( MIN_FREE_MB * 1024 + (iso_bytes + 1023) / 1024 ))
(( available_kb >= required_kb )) || fail "insufficient free space at $WORK_ROOT: need at least $((required_kb / 1024)) MiB including ISO and reserve"

"$QEMU_IMG" create -f qcow2 "$work_dir/guest.qcow2" "${GUEST_DISK_GB}G" >/dev/null
printf 'ISO_SHA256=%s\nWORK_DIR=%s\n' "$actual_sha" "$work_dir"
printf 'This is an ephemeral interactive guest. No host disk, network, shared folder, or passthrough device is attached.\n'

serial_args=()
if [[ -n "$QEMU_SERIAL_LOG" ]]; then
	serial_args=(-serial "file:$QEMU_SERIAL_LOG" -monitor none -display none)
fi

"$QEMU_SYSTEM" \
	-machine q35,accel="${QEMU_ACCEL:-tcg}" \
	-cpu max \
	-m "$GUEST_RAM_MB" \
	-smp "${GUEST_CPUS:-2}" \
	-drive "file=$work_dir/guest.qcow2,if=virtio,format=qcow2" \
	-cdrom "$GUEST_ISO" \
	-boot order=d \
	-nic none \
	-display "${QEMU_DISPLAY:-sdl}" \
	"${serial_args[@]}" \
	-no-reboot \
	-no-shutdown
