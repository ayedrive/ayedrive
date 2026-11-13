#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
set -euo pipefail

client="${1:?display_guest binary is required}"
output="${2:?output initramfs is required}"
root="$(mktemp -d)"
trap 'rm -rf "$root"' EXIT
mkdir -p "$root/bin" "$root/dev" "$root/proc" "$root/sys" "$root/etc"
cp "$client" "$root/bin/display_guest"
cp /usr/bin/busybox "$root/bin/busybox"
ln -s busybox "$root/bin/sh"
ln -s busybox "$root/bin/mount"
ln -s busybox "$root/bin/sleep"
cat > "$root/init" <<'INIT'
#!/bin/sh
mount -t proc proc /proc
mount -t sysfs sysfs /sys
mount -t devtmpfs devtmpfs /dev
echo 'DPC_GUEST_INIT_STARTED'
attempt=1
while [ "$attempt" -le 30 ]; do
	port=""
  for candidate in /dev/vport*; do
	if [ -e "$candidate" ]; then port="$candidate"; break; fi
  done
  if [ -n "$port" ]; then
	echo 'DPC_GUEST_VIRTIO_PORT_READY'
	/bin/display_guest "$port"
	status=$?
	echo "DPC_GUEST_DISPLAY_EXIT=$status"
	exit "$status"
  fi
  sleep 1
  attempt=$((attempt + 1))
done
echo 'DPC_GUEST_VIRTIO_PORT_TIMEOUT'
ls -la /dev /sys/class/virtio-ports 2>/dev/null || true
exit 4
INIT
chmod 0755 "$root/init"
mkdir -p "$(dirname "$output")"
(cd "$root" && find . -print | cpio -H newc -o 2>/dev/null | gzip -n > "$OLDPWD/$output")
printf 'created_initramfs=%s\n' "$output"
