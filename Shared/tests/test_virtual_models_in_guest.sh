#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.
set -euo pipefail

repo_root="${1:-$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)}"
cd "$repo_root"
make -C Shared test-virtual-model CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wstrict-prototypes -fno-common -O2'
make -C Linux user-client CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O2'
if [[ -e /dev/dpc-reference ]]; then
	./Linux/build/test_protocol
else
	printf '%s\n' 'Linux client compiled; live ioctl test skipped because /dev/dpc-reference is not present.'
fi # end /dev/dpc-reference availability check
printf '%s\n' 'GUEST_MODEL_TESTS_PASSED; device models are synthetic software tests, not physical-device conformance.'
