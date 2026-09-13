#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define HCI_TYPE_APPLICATION (hci_application_get_type())

G_DECLARE_FINAL_TYPE(HciApplication, hci_application, HCI, APPLICATION, AdwApplication)

HciApplication *hci_application_new(const char *application_id, GApplicationFlags flags);

G_END_DECLS
