#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define HCI_TYPE_PREFERENCES_WINDOW (hci_preferences_window_get_type())

G_DECLARE_FINAL_TYPE(HciPreferencesWindow, hci_preferences_window, HCI, PREFERENCES_WINDOW, AdwPreferencesDialog)

HciPreferencesWindow *hci_preferences_window_new(GtkWindow *parent);

G_END_DECLS
