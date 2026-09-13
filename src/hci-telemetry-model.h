#pragma once

#include <gio/gio.h>

G_BEGIN_DECLS

#define HCI_TYPE_TELEMETRY_ITEM (hci_telemetry_item_get_type())

G_DECLARE_FINAL_TYPE(HciTelemetryItem, hci_telemetry_item, HCI, TELEMETRY_ITEM, GObject)

HciTelemetryItem *hci_telemetry_item_new(const char *name,
                                         const char *value,
                                         const char *unit);

const char       *hci_telemetry_item_get_name(HciTelemetryItem *self);
void              hci_telemetry_item_set_name(HciTelemetryItem *self, const char *name);

const char       *hci_telemetry_item_get_value(HciTelemetryItem *self);
void              hci_telemetry_item_set_value(HciTelemetryItem *self, const char *value);

const char       *hci_telemetry_item_get_unit(HciTelemetryItem *self);
void              hci_telemetry_item_set_unit(HciTelemetryItem *self, const char *unit);

GListStore       *hci_telemetry_model_create_default(void);

G_END_DECLS
