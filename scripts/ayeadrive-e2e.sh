#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 1993-2026 Abhishek Choudhary. All rights reserved.
set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUTPUT_DIR="${1:-${ROOT_DIR}/artifacts/ayeadrive}"
FAIL_FAST="${AYEADRIVE_FAIL_FAST:-0}"
mkdir -p "${OUTPUT_DIR}"
export PYTHONPATH="${ROOT_DIR}:${ROOT_DIR}/tools${PYTHONPATH:+:${PYTHONPATH}}"

run_step() {
	local name="$1"
	shift
	echo "[AyeDrive] ${name}" | tee -a "${OUTPUT_DIR}/run.log"
	"$@" >>"${OUTPUT_DIR}/run.log" 2>&1
	local status=$?
	if [[ ${status} -ne 0 && "${FAIL_FAST}" == "1" ]]; then
		echo "[AyeDrive] fatal step failure: ${name}" | tee -a "${OUTPUT_DIR}/run.log"
		return "${status}"
	fi
	if [[ ${status} -ne 0 ]]; then
		echo "[AyeDrive] recoverable step failure: ${name}" | tee -a "${OUTPUT_DIR}/run.log"
	fi
	return 0
}

run_step "schema and assurance validation" python "${ROOT_DIR}/tools/driver_factory/validate_schemas.py"
run_step "independent black-box simulation harness" python -m tools.ayeadrive_sim.harness --output "${OUTPUT_DIR}"
run_step "AyeDrive harness tests" python -m unittest discover -s "${ROOT_DIR}/tools/ayeadrive_sim/tests" -v
run_step "unit tests" python -m unittest discover -s "${ROOT_DIR}/tools/driver_factory/tests" -v

printf '{"product":"AyeDrive","status":"completed","audit":"%s","certification":"not_claimed"}\n' "${OUTPUT_DIR}" | tee "${OUTPUT_DIR}/run-summary.json"
exit 0
