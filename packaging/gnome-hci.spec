Name:           gnome-hci
Version:        0.1.0
Release:        1%{?dist}
Summary:        Hardware Control and Telemetry Interface for Ivy Bridge & Wayland

License:        GPL-3.0-or-later
URL:            https://github.com/zaboal/gnome-hci
Source0:        %{name}-%{version}.tar.gz

BuildRequires:  meson >= 0.62.0
BuildRequires:  ninja-build
BuildRequires:  gcc
BuildRequires:  pkgconfig(gtk4) >= 4.14.0
BuildRequires:  pkgconfig(libadwaita-1) >= 1.5.0
BuildRequires:  pkgconfig(glib-2.0) >= 2.76.0
BuildRequires:  pkgconfig(gio-2.0) >= 2.76.0
BuildRequires:  pkgconfig(cairo) >= 1.16.0
BuildRequires:  pkgconfig(sqlite3) >= 3.35.0
BuildRequires:  desktop-file-utils
BuildRequires:  libappstream-glib

Requires:       gtk4 >= 4.14.0
Requires:       libadwaita >= 1.5.0
Requires:       hicolor-icon-theme

%description
GNOME HCI provides hardware control, microarchitectural telemetry monitoring,
thermal and fan regulation, and Wayland non-root display actuation tailored for Linux systems.

%prep
%autosetup

%build
%meson
%meson_build

%install
%meson_install

%check
%meson_test

%files
%license LICENSE
%doc README.md
%{_bindir}/gnome-hci
%{_datadir}/applications/org.gnome.Hci.desktop
%{_datadir}/metainfo/org.gnome.Hci.metainfo.xml
%{_datadir}/glib-2.0/schemas/org.gnome.Hci.gschema.xml

%changelog
* Sun Sep 13 2026 Bogdan Zažigin <git+me@zba.su> - 0.1.0-1
- Initial skeleton release of GNOME HCI
