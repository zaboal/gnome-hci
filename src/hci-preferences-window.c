#include "hci-preferences-window.h"

struct _HciPreferencesWindow {
    AdwPreferencesDialog parent_instance;
    AdwSpinRow *spin_sampling_interval;
    AdwSwitchRow *switch_sqlite_logging;
    AdwSwitchRow *switch_enforce_ac;
    AdwSwitchRow *switch_sata_lpm;
    GSettings *settings;
};

G_DEFINE_FINAL_TYPE(HciPreferencesWindow, hci_preferences_window, ADW_TYPE_PREFERENCES_DIALOG)

static void
hci_preferences_window_dispose(GObject *object)
{
    HciPreferencesWindow *self = HCI_PREFERENCES_WINDOW(object);

    g_clear_object(&self->settings);

    G_OBJECT_CLASS(hci_preferences_window_parent_class)->dispose(object);
}

static void
hci_preferences_window_class_init(HciPreferencesWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    object_class->dispose = hci_preferences_window_dispose;

    gtk_widget_class_set_template_from_resource(
        widget_class,
        "/org/gnome/Hci/ui/preferences.ui"
    );

    gtk_widget_class_bind_template_child(widget_class, HciPreferencesWindow, spin_sampling_interval);
    gtk_widget_class_bind_template_child(widget_class, HciPreferencesWindow, switch_sqlite_logging);
    gtk_widget_class_bind_template_child(widget_class, HciPreferencesWindow, switch_enforce_ac);
    gtk_widget_class_bind_template_child(widget_class, HciPreferencesWindow, switch_sata_lpm);
}

static void
hci_preferences_window_init(HciPreferencesWindow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));

    GSettingsSchemaSource *source = g_settings_schema_source_get_default();
    if (source) {
        g_autoptr(GSettingsSchema) schema = g_settings_schema_source_lookup(source, "org.gnome.Hci", TRUE);
        if (schema) {
            self->settings = g_settings_new("org.gnome.Hci");
            g_settings_bind(
                self->settings, "sampling-interval-ms",
                self->spin_sampling_interval, "value",
                G_SETTINGS_BIND_DEFAULT
            );
            g_settings_bind(
                self->settings, "latency-performance-mode",
                self->switch_enforce_ac, "active",
                G_SETTINGS_BIND_DEFAULT
            );
        }
    }
}

HciPreferencesWindow *
hci_preferences_window_new(GtkWindow *parent)
{
    (void)parent;
    return g_object_new(
        HCI_TYPE_PREFERENCES_WINDOW,
        NULL
    );
}
