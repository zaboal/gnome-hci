# GNOME HCI

**GNOME HCI** is a modern GTK4 and Libadwaita hardware control and telemetry interface designed for Linux systems, optimized for Ivy Bridge architecture (ThinkPad T430) and Wayland environments.

## Architecture & Principles

- **Pure GTK4 + Libadwaita (GTK 4-to-5 Migration Compliant)**:
  - Zero deprecated APIs (`GtkTreeView` and `GtkIconView` forbidden).
  - High-performance custom widget rendering via `GtkSnapshot` (`HciGauge`).
  - Declarative lists and tabular telemetry through `GtkColumnView` and `GListStore`.
  - Responsive adaptive layout utilizing `AdwNavigationSplitView` and `AdwPreferencesWindow`.
- **System Telemetry & Control**:
  - Microarchitectural IPC and CPU frequency telemetry tracking.
  - ThinkPad ACPI acoustic fan control and AC mains latency-performance enforcement.
  - Non-root Wayland display actuation via Mutter D-Bus APIs.
  - Local time-series metric logging backed by SQLite3.

## Building & Installation

### Build Dependencies

- `meson >= 0.62.0`
- `ninja-build`
- `gcc`
- `gtk4 >= 4.14.0`
- `libadwaita-1 >= 1.5.0`
- `glib-2.0 >= 2.76.0`
- `gio-2.0 >= 2.76.0`
- `cairo >= 1.16.0`
- `sqlite3 >= 3.35.0`

### Build Instructions

```bash
meson setup build
meson compile -C build
meson test -C build
```

## Fedora COPR Packaging

Cloud-compiled Ivy Bridge RPMs are built in COPR at:
`zaboal/t430-optimized`

## License

GPL-3.0-or-later.
