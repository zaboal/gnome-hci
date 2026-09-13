# GNOME HCI

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/richardlitt/standard-readme)
[![CI](https://github.com/zaboal/gnome-hci/actions/workflows/ci.yml/badge.svg)](https://github.com/zaboal/gnome-hci/actions/workflows/ci.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

> Modern GTK4 and Libadwaita hardware control and system telemetry interface for Linux Wayland desktops.

GNOME HCI provides low-overhead hardware regulation, microarchitectural performance telemetry, and non-root Wayland display actuation tailored for responsive desktop performance.

## Table of Contents

- [Background](#background)
- [Install](#install)
- [Usage](#usage)
- [API & Architecture](#api--architecture)
- [Contributing](#contributing)
- [Maintainers](#maintainers)
- [License](#license)

## Background

Modern Linux desktop environments on resource-constrained hardware require granular hardware controls and immediate microarchitectural visibility without incurring heavy background polling penalties. GNOME HCI was engineered to address this balance with specific operational imperatives:

- **Ivy Bridge T430 Microarchitecture Context**: Designed and calibrated for Intel Ivy Bridge workstations (such as the Lenovo ThinkPad T430 / T430s featuring Intel Core i5-3320M, 2C/4T, Intel HD Graphics 4000). The Ivy Bridge microarchitecture features 32 KB L1i, 32 KB L1d, 256 KB L2 per core, and a shared 3 MB L3 cache. While AVX1 supports 256-bit floating-point registers (`%ymm`), integer SIMD remains 128-bit. Tracking empirical Instructions Per Cycle (IPC) alongside core frequencies exposes when workloads are compute-bound versus stalled on memory or cache misses, guiding optimal execution without causing instruction-cache bloat.
- **Mutter D-Bus Display Actuation**: Under GNOME Shell on Wayland, manipulating display properties through direct kernel sysfs (`/sys/class/backlight/...`) either requires elevated root privileges or causes race conditions with the compositor. GNOME HCI interacts seamlessly with the desktop session through unprivileged Mutter session D-Bus interfaces: `org.gnome.Mutter.DisplayConfig` for brightness actuation and `org.gnome.settings-daemon.plugins.color` for color temperature adjustments. In addition, `org.gnome.Mutter.IdleMonitor` is monitored to suspend or throttle active telemetry sampling cycles whenever the user is idle, preserving system cycles.
- **AC Mains Power Latency Tuning**: Calibrated for workstations operating continuously on AC mains power. GNOME HCI manages the enforcement of `tuned-adm profile latency-performance` and SATA Link Power Management (LPM) `max_performance` to eliminate latency-inducing state transitions. Thermal safeguards coordinate with ThinkPad ACPI fan control (`/proc/acpi/ibm/fan`) to engage unrestricted fan boost (`level disengaged`), preventing thermal throttling and maintaining sustained 3.3 GHz Turbo Boost frequencies. Dirty page writeback intervals are preserved at 5 seconds (`vm.dirty_writeback_centisecs = 500`) to guarantee data integrity against abrupt power loss.
- **GTK 4-to-5 Migration Principles**: Written in pure C11 using GTK4 and Libadwaita with zero deprecated APIs. Legacy GTK2/3 constructs such as `GtkTreeView` and `GtkIconView` are strictly avoided. Custom visual components render directly to `GtkSnapshot` node trees, and tabular metrics leverage declarative `GtkColumnView` factories over `GListModel` stores to guarantee clean future migration toward GTK5.

## Install

### System Prerequisites

GNOME HCI is targeted for Fedora Linux (Fedora 40+) running GNOME Shell on Wayland, but can be built on any modern Linux distribution satisfying the following toolchain and library dependencies:

- `meson` (>= 0.62.0)
- `ninja-build`
- `gcc` (C11 standard support)
- `gtk4-devel` (>= 4.14.0)
- `libadwaita-devel` (>= 1.5.0)
- `glib2-devel` (>= 2.76.0)
- `cairo-devel` (>= 1.16.0)
- `sqlite-devel` (>= 3.35.0)

Install all prerequisites on Fedora:

```bash
sudo dnf install -y \
  meson \
  ninja-build \
  gcc \
  gtk4-devel \
  libadwaita-devel \
  glib2-devel \
  cairo-devel \
  sqlite-devel
```

### Build Commands

To configure, compile, test, and install GNOME HCI from source:

```bash
# Setup Meson build configuration
meson setup build

# Compile application binaries and resources
meson compile -C build

# Execute test suite
meson test -C build

# Install binary, desktop entry, and schemas
sudo meson install -C build
```

## Usage

### Running the Application

Launch the installed binary from the terminal:

```bash
gnome-hci
```

To run directly from a local build tree without installation:

```bash
./build/src/gnome-hci
```

### CLI Flags and Options

GNOME HCI supports standard GApplication command-line options:

- `--help`, `-h`: Show summary of available command-line options.
- `--version`: Output the current application version and build metadata.
- `--gapplication-service`: Start the application as a D-Bus background service for auto-activation.

### Desktop Entry Integration

When installed to system directories, GNOME HCI integrates with the desktop shell via:

- **Desktop File**: `org.gnome.Hci.desktop` (installed to `/usr/share/applications/`), registering the app under `GNOME`, `GTK`, `Settings`, and `HardwareSettings`.
- **AppStream Metainfo**: `org.gnome.Hci.metainfo.xml` (installed to `/usr/share/metainfo/`).
- **GSettings Schema**: `org.gnome.Hci.gschema.xml` (installed to `/usr/share/glib-2.0/schemas/`), managing window state, polling intervals, and latency tuning modes.

## API & Architecture

The application is structured into modular GObject and Libadwaita subsystems:

### GTK4 / Libadwaita UI Components

- **`HciApplication` (`src/hci-application.c`)**: Subclasses `AdwApplication`. Manages application lifecycle, GSettings state persistence, and global actions (`app.preferences`, `app.about`, `app.quit`).
- **`HciWindow` (`src/hci-window.c`)**: Subclasses `AdwApplicationWindow`. Hosts an `AdwNavigationSplitView` presenting primary controls (latency switches, fan overrides) alongside high-frequency telemetry gauges.
- **`HciGauge` (`src/hci-gauge.c`)**: Custom hardware metric gauge subclassing `GtkWidget`. Overrides the `snapshot()` virtual method to render anti-aliased arcs via `GtkSnapshot` and Cairo (`gtk_snapshot_append_cairo`) with fractional updates.
- **`HciTelemetryModel` (`src/hci-telemetry-model.c`)**: Backs `GtkColumnView` using `GListStore` and `GtkSignalListItemFactory`. Connects item setup and bind handlers to present live metrics (CPU frequency, package temperature, IPC, fan RPM) without UI thread stalling.
- **`HciPreferencesWindow` (`src/hci-preferences-window.c`)**: Subclasses `AdwPreferencesWindow`, providing user configuration for sampling intervals and AC latency enforcement.

### D-Bus Integration

- **`org.gnome.Mutter.DisplayConfig`**: Queries display layout state and issues brightness adjustments over the user session D-Bus without requiring elevated privileges.
- **`org.gnome.Mutter.IdleMonitor`**: Subscribes to Mutter idle signals to adjust telemetry polling frequency dynamically when the system is inactive.
- **System Power & Cooling**: Interacts with tuned latency-performance profiles and ThinkPad ACPI fan interfaces (`/proc/acpi/ibm/fan`) to prevent thermal throttling.

### Local SQLite Telemetry Persistence Schema

Metrics logged during monitoring sessions are recorded in a lightweight SQLite database located at `$XDG_DATA_HOME/gnome-hci/telemetry.db`:

```sql
CREATE TABLE IF NOT EXISTS telemetry_samples (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  timestamp INTEGER NOT NULL,
  metric TEXT NOT NULL,
  value REAL NOT NULL,
  unit TEXT
);
```

- **`id`**: Unique sequential sample identifier.
- **`timestamp`**: Unix epoch timestamp in seconds.
- **`metric`**: Metric identifier key (e.g. `cpu_ipc`, `cpu_freq_mhz`, `fan_rpm`, `pkg_temp_c`).
- **`value`**: Floating-point telemetry sample value.
- **`unit`**: Display unit descriptor (e.g. `IPC`, `MHz`, `RPM`, `°C`).

## Contributing

Contributions are welcome! Please follow these guidelines:

1. **Branch Discipline**: Create focused feature or bugfix branches from `main` (e.g. `feat/new-sensor`, `fix/db-locking`).
2. **Conventional Commits**: Write clear, semantic commit messages conforming to the [Conventional Commits](https://www.conventionalcommits.org/) standard (e.g. `feat:`, `fix:`, `docs:`, `perf:`).
3. **Cryptographic Signing**: All git commits must be signed using GPG (`git commit -S`).
4. **Code Quality**: Ensure C code compiles cleanly with `-Dwarning_level=2` and `-Dwerror=true` without compiler warnings.
5. **Validation**: Run `meson test -C build` and validate desktop and schema assets prior to opening a pull request.

## Maintainers

Bogdan Zažigin <git+me@zba.su>

## License

[GPL-3.0-or-later](https://www.gnu.org/licenses/gpl-3.0) (GNU General Public License v3.0 or later).
