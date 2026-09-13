#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define HCI_TYPE_GAUGE (hci_gauge_get_type())

G_DECLARE_FINAL_TYPE(HciGauge, hci_gauge, HCI, GAUGE, GtkWidget)

GtkWidget   *hci_gauge_new(void);

void         hci_gauge_set_fraction(HciGauge *self, double fraction);
double       hci_gauge_get_fraction(HciGauge *self);

void         hci_gauge_set_label(HciGauge *self, const char *label);
const char  *hci_gauge_get_label(HciGauge *self);

G_END_DECLS
