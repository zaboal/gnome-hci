#pragma once

#include <glib-object.h>
#include <sqlite3.h>

G_BEGIN_DECLS

#define HCI_TYPE_DATABASE (hci_database_get_type())

G_DECLARE_FINAL_TYPE(HciDatabase, hci_database, HCI, DATABASE, GObject)

HciDatabase *hci_database_new(void);

gboolean     hci_database_record_metric(HciDatabase *self,
                                        const char  *metric_name,
                                        double       metric_value,
                                        const char  *unit);

G_END_DECLS
