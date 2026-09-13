#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define HCI_TYPE_WINDOW (hci_window_get_type())

G_DECLARE_FINAL_TYPE(HciWindow, hci_window, HCI, WINDOW, AdwApplicationWindow)

HciWindow *hci_window_new(GtkApplication *app);

G_END_DECLS
