# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
Push-Location $repoRoot
try {
	python -m pip install -r tools/requirements-ci.txt
	if ($LASTEXITCODE -ne 0) { throw 'Python dependency installation failed.' }

	python tools/driver_factory/validate_schemas.py
	if ($LASTEXITCODE -ne 0) { throw 'Schema validation failed.' }
	$env:PYTHONPATH = Join-Path $repoRoot 'tools'
	python -m driver_factory assurance-validate DriverTest/models/assurance-package.json
	if ($LASTEXITCODE -ne 0) { throw 'Assurance package validation failed.' }

	python -m unittest discover -s DriverTest/tests -v
	if ($LASTEXITCODE -ne 0) { throw 'DriverTest Python suite failed.' }

	python -m unittest discover -s Shared/tests -p test_virtual_profiles.py -v
	if ($LASTEXITCODE -ne 0) { throw 'Virtual-device profile validation failed.' }

	$env:PYTHONPATH = Join-Path $repoRoot 'tools'
	python -m unittest discover -s tools/driver_factory/tests -v
	if ($LASTEXITCODE -ne 0) { throw 'Driver factory Python suite failed.' }

	$evidenceRoot = Join-Path $repoRoot 'build/dossier'
	if (Test-Path $evidenceRoot) { Remove-Item -Recurse -Force $evidenceRoot }
	python -m driver_factory dossier-package DriverTest/models/device-dossier.template.json --out $evidenceRoot
	if ($LASTEXITCODE -ne 0) { throw 'Dossier package generation failed.' }
	python -m driver_factory evidence-index $evidenceRoot --out (Join-Path $evidenceRoot 'evidence-index.json')
	if ($LASTEXITCODE -ne 0) { throw 'Evidence index generation failed.' }

	$manifestPath = Join-Path $evidenceRoot 'provenance.manifest.json'
	python -m driver_factory attest $evidenceRoot --out $manifestPath
	if ($LASTEXITCODE -ne 0) { throw 'Evidence inventory creation failed.' }
	python -m driver_factory verify-attestation $evidenceRoot $manifestPath
	if ($LASTEXITCODE -ne 0) { throw 'Evidence inventory verification failed.' }

	$env:PYTHONPATH = "${repoRoot};${repoRoot}\tools"
	python -m driver_factory.site docs/_site
	if ($LASTEXITCODE -ne 0) { throw 'Static site build failed.' }

	$strictCFlags = '-std=c11 -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow -Wstrict-prototypes -fno-common -O2'
	$strictCxxFlags = '-std=c++17 -Wall -Wextra -Wpedantic -Werror -O2'
	if (Get-Command make -ErrorAction SilentlyContinue) {
		make -C Shared clean test "CFLAGS=$strictCFlags" "CXXFLAGS=$strictCxxFlags"
		if ($LASTEXITCODE -ne 0) { throw 'Portable C/C++ tests failed.' }
	}
	elseif (Get-Command wsl.exe -ErrorAction SilentlyContinue) {
		$wslPath = (& wsl.exe -- wslpath -a -u $repoRoot).Trim()
		if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($wslPath)) { throw 'Could not resolve the repository path inside WSL.' }
		$wslCommand = "cd '$wslPath' && make -C Shared clean test CFLAGS='$strictCFlags' CXXFLAGS='$strictCxxFlags'"
		& wsl.exe -- bash -lc $wslCommand
		if ($LASTEXITCODE -ne 0) { throw 'Portable C/C++ tests failed in WSL.' }
	}
	else {
		throw 'Portable C/C++ tests require GNU make and C/C++ compilers, either natively or in WSL.'
	}

	Write-Output 'LOCAL_CI_PASSED; native SDK, QEMU guest, HIL, Pages deployment, and product approval remain separate gates.'
}
finally {
	Pop-Location
}
