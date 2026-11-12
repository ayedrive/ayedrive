#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
set -euo pipefail

: "${PACKAGE_DIR:?Set PACKAGE_DIR to the built driver package directory}"
OUTPUT_ISO="${OUTPUT_ISO:-$PWD/driver-test-media.iso}"
TEST_CERT="${TEST_CERT:-}"
if [[ ! -d "$PACKAGE_DIR" ]]; then
	echo "Driver package directory not found: $PACKAGE_DIR" >&2
	exit 2
fi
command -v xorriso >/dev/null || { echo "Install xorriso in WSL first: sudo apt install xorriso" >&2; exit 2; }

DRIVER_TEST_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
STAGING="$(mktemp -d)"
trap 'rm -rf "$STAGING"' EXIT
mkdir -p "$STAGING/DriverPackage" "$STAGING/DriverTest/models"
cp -a "$PACKAGE_DIR/." "$STAGING/DriverPackage/"
cp "$DRIVER_TEST_DIR/driver_test.py" "$STAGING/DriverTest/"
cp "$DRIVER_TEST_DIR/models/umdf2-root.json" "$STAGING/DriverTest/models/"
cp "$DRIVER_TEST_DIR/models/device-classes.json" "$STAGING/DriverTest/models/"
if [[ -n "$TEST_CERT" ]]; then
	[[ -f "$TEST_CERT" ]] || { echo "Test certificate not found: $TEST_CERT" >&2; exit 2; }
	cp "$TEST_CERT" "$STAGING/DriverPackage/"
fi
mkdir -p "$(dirname "$OUTPUT_ISO")"
xorriso -as mkisofs -J -R -V DRIVER_TEST -o "$OUTPUT_ISO" "$STAGING" >/dev/null
printf 'Created test media: %s\n' "$OUTPUT_ISO"
