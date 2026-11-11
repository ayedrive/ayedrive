#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
set -euo pipefail

: "${WIN_VM_DISK:?Set WIN_VM_DISK to the path of an existing Windows 11 qcow2 disk}"

QEMU_BIN="${QEMU_BIN:-qemu-system-x86_64}"
OVMF_CODE="${OVMF_CODE:-/usr/share/OVMF/OVMF_CODE_4M.fd}"
OVMF_VARS_TEMPLATE="${OVMF_VARS_TEMPLATE:-/usr/share/OVMF/OVMF_VARS_4M.fd}"
WIN_VM_VARS="${WIN_VM_VARS:-$HOME/.local/share/driver-test/OVMF_VARS.fd}"
VM_MEMORY="${VM_MEMORY:-8192}"
VM_CPUS="${VM_CPUS:-4}"
TEST_MEDIA_ISO="${TEST_MEDIA_ISO:-}"
WINDOWS_INSTALL_ISO="${WINDOWS_INSTALL_ISO:-}"

if [[ ! -f "$WIN_VM_DISK" ]]; then
	echo "Windows VM disk not found: $WIN_VM_DISK" >&2
	exit 2
fi
if [[ ! -f "$OVMF_CODE" || ! -f "$OVMF_VARS_TEMPLATE" ]]; then
	echo "OVMF firmware files not found. Set OVMF_CODE and OVMF_VARS_TEMPLATE." >&2
	exit 2
fi

mkdir -p "$(dirname "$WIN_VM_VARS")"
if [[ ! -f "$WIN_VM_VARS" ]]; then
	cp "$OVMF_VARS_TEMPLATE" "$WIN_VM_VARS"
fi

if [[ -n "${QEMU_ACCEL:-}" ]]; then
	ACCEL="$QEMU_ACCEL"
elif [[ -e /dev/kvm ]]; then
	ACCEL=kvm
else
	ACCEL=tcg
fi
if [[ "$ACCEL" == "kvm" ]]; then
	CPU=host
else
	CPU=max
fi

QEMU_ARGS=(
	-name "${VM_NAME:-driver-test-windows}"
	-machine "q35,accel=$ACCEL"
	-cpu "$CPU"
	-smp "$VM_CPUS"
	-m "$VM_MEMORY"
	-drive "if=pflash,format=raw,readonly=on,file=$OVMF_CODE"
	-drive "if=pflash,format=raw,file=$WIN_VM_VARS"
	-drive "if=none,id=osdisk,format=qcow2,file=$WIN_VM_DISK"
	-device ich9-ahci,id=sata
	-device ide-hd,drive=osdisk,bus=sata.0
	-netdev user,id=net0
	-device e1000e,netdev=net0
	-device qemu-xhci,id=xhci
)

if [[ -n "$TEST_MEDIA_ISO" ]]; then
	if [[ ! -f "$TEST_MEDIA_ISO" ]]; then
		echo "Test media ISO not found: $TEST_MEDIA_ISO" >&2
		exit 2
	fi
fi
if [[ -n "$WINDOWS_INSTALL_ISO" ]]; then
	if [[ ! -f "$WINDOWS_INSTALL_ISO" ]]; then
		echo "Windows installation ISO not found: $WINDOWS_INSTALL_ISO" >&2
		exit 2
	fi
	QEMU_ARGS+=(-drive "if=none,id=windowsinstall,media=cdrom,readonly=on,file=$WINDOWS_INSTALL_ISO")
	QEMU_ARGS+=(-device ide-cd,drive=windowsinstall,bus=sata.1)
	QEMU_ARGS+=(-boot order=dc,menu=on)
fi
if [[ -n "$TEST_MEDIA_ISO" ]]; then
	QEMU_ARGS+=(-drive "if=none,id=testmedia,media=cdrom,readonly=on,file=$TEST_MEDIA_ISO")
	QEMU_ARGS+=(-device ide-cd,drive=testmedia,bus=sata.2)
fi

if [[ "${QEMU_TPM:-0}" == "1" ]]; then
	command -v swtpm >/dev/null || { echo "Install swtpm in WSL or set QEMU_TPM=0." >&2; exit 2; }
	TPM_STATE="${TPM_STATE:-$HOME/.local/share/driver-test/swtpm}"
	TPM_SOCKET="${TPM_SOCKET:-$TPM_STATE/swtpm.sock}"
	TPM_PID="$TPM_STATE/swtpm.pid"
	mkdir -p "$TPM_STATE"
	rm -f "$TPM_SOCKET" "$TPM_PID"
	swtpm socket --tpm2 --tpmstate "dir=$TPM_STATE" --ctrl "type=unixio,path=$TPM_SOCKET" --daemon --pid "file=$TPM_PID"
	cleanup_tpm() {
		if [[ -f "$TPM_PID" ]]; then
			kill "$(cat "$TPM_PID")" 2>/dev/null || true
			rm -f "$TPM_PID" "$TPM_SOCKET"
		fi
	}
	trap cleanup_tpm EXIT
	QEMU_ARGS+=(-chardev "socket,id=chrtpm,path=$TPM_SOCKET" -tpmdev emulator,id=tpm0,chardev=chrtpm -device tpm-tis,tpmdev=tpm0)
fi

if [[ -n "${QEMU_EXTRA_ARGS:-}" ]]; then
	read -r -a EXTRA_ARGS <<< "$QEMU_EXTRA_ARGS"
	QEMU_ARGS+=("${EXTRA_ARGS[@]}")
fi

"$QEMU_BIN" "${QEMU_ARGS[@]}"
