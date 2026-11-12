# Windows driver test infrastructure

SPDX-License-Identifier: GPL-3.0-only  
Copyright (C) 1993-2026 Abhishek Choudhary. All rights reserved.

This folder provides a dependency-free Python harness, JSON device-model manifests, and a WSL/QEMU launcher. The harness is VM-independent for manifest validation and reporting; the built-in Windows device-I/O adapter must run inside a Windows guest.

## Current coverage

The included runnable model targets the repository's `UMDF2Driver1` software-enumerated device. It discovers the configured Windows device-interface GUID, opens the first matching interface, sends a configured IOCTL, and writes a JSON report. The sample driver's callback currently completes every IOCTL with success, so this is only an enumeration/open/dispatch smoke test—not validation of real device behavior.

The harness supports generic `ioctl`, `read`, and `write` operations against Windows device interfaces. Writes are blocked unless `--allow-write` is explicitly supplied. Use disposable VM disks and never point a write test at a host disk or production device.

## Validate and run the sample

From the repository root, validate the manifest on Windows, WSL, or another Python 3.10+ host:

```powershell
python DriverTest/driver_test.py list-transports
python DriverTest/driver_test.py validate DriverTest/models/umdf2-root.json
python -m unittest discover -s DriverTest/tests -v
```

Build the driver in Visual Studio using `Debug|x64`. The driver package is under `UMDF 2 Driver1/x64/Debug/UMDF 2 Driver1`; the generated test certificate is `UMDF 2 Driver1/x64/Debug/UMDF2Driver1.cer`.

Run live tests only in the Windows guest:

1. Enable test signing inside the guest from an elevated prompt with `bcdedit /set testsigning on`, then reboot. If Secure Boot blocks test mode, disable Secure Boot in the VM firmware settings. Do not enable test signing on the host.
2. Import the package's `.cer` into the guest's Local Machine `Trusted Root Certification Authorities` and `Trusted Publishers` stores.
3. Install and create the root-enumerated device. With the WDK `devcon` tool, run `devcon install D:\DriverPackage\UMDF2Driver1.inf Root\UMDF2Driver1` from an elevated prompt. Devcon is required here because this host ID is a software-enumerated root device; `pnputil /add-driver` only stages/installs a package for matching devices and does not create this instance.
4. Install Python 3.10+ in the guest, insert the test-media ISO, and run from its mounted drive:

```powershell
python D:\DriverTest\driver_test.py test D:\DriverTest\models\umdf2-root.json --report D:\driver-test-report.json
```

The test report is JSON and includes interface discovery, open, and I/O operation results. If the device is not present or the guest's interface GUID differs from the manifest, discovery fails rather than silently passing.

## QEMU under WSL

The launcher uses an existing Windows guest disk; it does not download Windows or create a device-specific hardware emulator. Obtain Windows installation media through the appropriate licensed channel. Run QEMU from WSL2. If `/dev/kvm` is available the launcher uses KVM, otherwise it falls back to QEMU TCG, which can be very slow for Windows.

Install the WSL packages:

```bash
sudo apt update
sudo apt install qemu-system-x86 ovmf xorriso swtpm
```

Build the driver in Visual Studio, then create an ISO containing the package, certificate, runner, and sample manifest. Run this from WSL (adjust paths to your checkout/build output):

```bash
cd "/mnt/c/Users/kaiva/source/repos/UMDF 2 Driver1"
PACKAGE_DIR="/mnt/c/Users/kaiva/source/repos/UMDF 2 Driver1/UMDF 2 Driver1/x64/Debug/UMDF 2 Driver1" \
TEST_CERT="/mnt/c/Users/kaiva/source/repos/UMDF 2 Driver1/UMDF 2 Driver1/x64/Debug/UMDF2Driver1.cer" \
OUTPUT_ISO="$HOME/driver-test-media.iso" \
bash DriverTest/qemu/make-test-media.sh
```

Create a disposable qcow2 disk and install Windows 11 from an ISO (the disk is user-provided; the launcher never modifies a host disk):

```bash
qemu-img create -f qcow2 "$HOME/win11-driver-test.qcow2" 80G
WIN_VM_DISK="$HOME/win11-driver-test.qcow2" \
WINDOWS_INSTALL_ISO="$HOME/Windows11.iso" \
TEST_MEDIA_ISO="$HOME/driver-test-media.iso" \
QEMU_TPM=1 \
bash DriverTest/qemu/run-windows-guest.sh
```

Complete Windows setup, install Python and the driver, and enable test signing in the guest. The launcher uses OVMF UEFI, QEMU's emulated e1000e network adapter and xHCI controller, and an optional software TPM. It keeps UEFI variables and TPM state under `~/.local/share/driver-test` by default. After Windows setup, launch the same disk without `WINDOWS_INSTALL_ISO` to boot the installed guest. `WIN_VM_DISK` must refer to a qcow2 disk; use a separate disk for each experiment or checkpoint it before driver changes.

Set `TEST_MEDIA_ISO` to attach the generated harness/package ISO. `QEMU_EXTRA_ARGS` can add trusted, model-specific QEMU command-line options. The launcher does not expose host block devices by default. Do not pass a physical disk to QEMU or run destructive block tests on host storage.

## Device models and extension points

`models/device-classes.json` catalogs the requested root, USB, Bluetooth, Wi-Fi, PCI, storage/block, network, robot, accelerator, and custom profiles. A manifest records the driver framework, transport, hardware IDs, interface GUID, adapter, and expected I/O operations. Add concrete device manifests only when the device identity, protocol, and expected behavior are known.

The built-in `windows-interface` adapter works for any Windows driver that exposes a device interface and supports ordinary handle-based I/O. It does not configure PCI BARs, emulate USB descriptors/endpoints, speak Bluetooth HCI, emulate Wi-Fi radio/NDIS behavior, or reproduce robot/accelerator semantics. Those require a transport-specific simulator or a device-specific QEMU model/physical passthrough plus an adapter that implements that device's protocol and assertions.

QEMU's generic e1000e/virtio network devices are Ethernet NICs, not Wi-Fi radios. PCI bus emulation is not a model of an arbitrary PCI function. USB passthrough requires the host device to be accessible to WSL/QEMU and can detach it from the host. Storage tests must use an expendable qcow2 image. Robot and accelerator tests need explicit models for timing, DMA, reset, sensors/actuators, and failure modes; they cannot be generated faithfully from a device name alone.

To add a test operation, extend the manifest validation and `WindowsInterfaceAdapter.execute` operation handling in `driver_test.py`, add a unit test under `tests`, and document any destructive opt-in. To add an emulator backend, add a transport-specific adapter/model rather than treating a generic QEMU device as equivalent to physical hardware.
