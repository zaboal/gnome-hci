project('gnome-hci', 'c',
  version: '0.1.0',
  meson_version: '>= 0.62.0',
  default_options: [
    'warning_level=2',
    'c_std=c11',
  ],
)

gnome = import('gnome')
i18n = import('i18n')

prefix = get_option('prefix')
bindir = prefix / get_option('bindir')
datadir = prefix / get_option('datadir')
localedir = prefix / get_option('localedir')

glib_dep = dependency('glib-2.0', version: '>= 2.76.0')
gio_dep = dependency('gio-2.0', version: '>= 2.76.0')
gtk_dep = dependency('gtk4', version: '>= 4.14.0')
adw_dep = dependency('libadwaita-1', version: '>= 1.5.0')
cairo_dep = dependency('cairo', version: '>= 1.16.0')
sqlite_dep = dependency('sqlite3', version: '>= 3.35.0')

hci_deps = [
  glib_dep,
  gio_dep,
  gtk_dep,
  adw_dep,
  cairo_dep,
  sqlite_dep,
]

config_h = configuration_data()
config_h.set_quoted('APP_ID', 'org.gnome.Hci')
config_h.set_quoted('PACKAGE_NAME', meson.project_name())
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('GETTEXT_PACKAGE', meson.project_name())
config_h.set_quoted('LOCALEDIR', localedir)
configure_file(
  output: 'config.h',
  configuration: config_h,
)

add_project_arguments([
  '-DAPPLICATION_ID="org.gnome.Hci"',
  '-I' + meson.project_build_root(),
  '-Wno-error=deprecated-declarations',
], language: 'c')

subdir('po')
subdir('data')
subdir('src')
