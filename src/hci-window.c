#include "hci-window.h"
#include "hci-gauge.h"
#include "hci-telemetry-model.h"

struct _HciWindow {
    AdwApplicationWindow parent_instance;

    AdwNavigationSplitView *split_view;
    HciGauge *cpu_gauge;
    AdwSwitchRow *latency_switch;
    AdwSwitchRow *fan_boost_switch;
    GtkColumnView *telemetry_view;

    GSettings *settings;
    GListStore *telemetry_store;
    guint telemetry_timeout_id;
};

G_DEFINE_FINAL_TYPE(HciWindow, hci_window, ADW_TYPE_APPLICATION_WINDOW)

static void
factory_setup_label(GtkSignalListItemFactory *factory,
                    GtkListItem              *list_item,
                    gpointer                  user_data)
{
    (void)factory;
    (void)user_data;

    GtkWidget *label = gtk_label_new(NULL);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_list_item_set_child(list_item, label);
}

static void
factory_bind_name(GtkSignalListItemFactory *factory,
                  GtkListItem              *list_item,
                  gpointer                  user_data)
{
    (void)factory;
    (void)user_data;

    GtkWidget *label = gtk_list_item_get_child(list_item);
    HciTelemetryItem *item = gtk_list_item_get_item(list_item);

    if (HCI_IS_TELEMETRY_ITEM(item)) {
        gtk_label_set_text(GTK_LABEL(label), hci_telemetry_item_get_name(item));
    }
}

static void
factory_bind_value(GtkSignalListItemFactory *factory,
                   GtkListItem              *list_item,
                   gpointer                  user_data)
{
    (void)factory;
    (void)user_data;

    GtkWidget *label = gtk_list_item_get_child(list_item);
    HciTelemetryItem *item = gtk_list_item_get_item(list_item);

    if (HCI_IS_TELEMETRY_ITEM(item)) {
        gtk_label_set_text(GTK_LABEL(label), hci_telemetry_item_get_value(item));
    }
}

static void
factory_bind_unit(GtkSignalListItemFactory *factory,
                  GtkListItem              *list_item,
                  gpointer                  user_data)
{
    (void)factory;
    (void)user_data;

    GtkWidget *label = gtk_list_item_get_child(list_item);
    HciTelemetryItem *item = gtk_list_item_get_item(list_item);

    if (HCI_IS_TELEMETRY_ITEM(item)) {
        gtk_label_set_text(GTK_LABEL(label), hci_telemetry_item_get_unit(item));
    }
}

static void
setup_column_view(HciWindow *self)
{
    /* Column 1: Metric Name */
    GtkListItemFactory *factory_name = gtk_signal_list_item_factory_new();
    g_signal_connect(factory_name, "setup", G_CALLBACK(factory_setup_label), NULL);
    g_signal_connect(factory_name, "bind", G_CALLBACK(factory_bind_name), NULL);

    GtkColumnViewColumn *col_name = gtk_column_view_column_new("Metric", factory_name);
    gtk_column_view_column_set_expand(col_name, TRUE);
    gtk_column_view_append_column(self->telemetry_view, col_name);

    /* Column 2: Value */
    GtkListItemFactory *factory_value = gtk_signal_list_item_factory_new();
    g_signal_connect(factory_value, "setup", G_CALLBACK(factory_setup_label), NULL);
    g_signal_connect(factory_value, "bind", G_CALLBACK(factory_bind_value), NULL);

    GtkColumnViewColumn *col_value = gtk_column_view_column_new("Value", factory_value);
    gtk_column_view_column_set_expand(col_value, FALSE);
    gtk_column_view_append_column(self->telemetry_view, col_value);

    /* Column 3: Unit */
    GtkListItemFactory *factory_unit = gtk_signal_list_item_factory_new();
    g_signal_connect(factory_unit, "setup", G_CALLBACK(factory_setup_label), NULL);
    g_signal_connect(factory_unit, "bind", G_CALLBACK(factory_bind_unit), NULL);

    GtkColumnViewColumn *col_unit = gtk_column_view_column_new("Unit", factory_unit);
    gtk_column_view_column_set_expand(col_unit, FALSE);
    gtk_column_view_append_column(self->telemetry_view, col_unit);

    /* Attach list store wrapped in single selection */
    self->telemetry_store = hci_telemetry_model_create_default();
    g_autoptr(GtkSingleSelection) selection = gtk_single_selection_new(G_LIST_MODEL(self->telemetry_store));
    gtk_column_view_set_model(self->telemetry_view, GTK_SELECTION_MODEL(selection));
}

static gboolean
on_telemetry_tick(gpointer user_data)
{
    HciWindow *self = HCI_WINDOW(user_data);

    /* Cycle or update gauge fraction slightly for live UI responsiveness */
    double current = hci_gauge_get_fraction(self->cpu_gauge);
    double next = current + 0.05;
    if (next > 0.95)
        next = 0.50;

    hci_gauge_set_fraction(self->cpu_gauge, next);

    return G_SOURCE_CONTINUE;
}

static void
hci_window_dispose(GObject *object)
{
    HciWindow *self = HCI_WINDOW(object);

    if (self->telemetry_timeout_id != 0) {
        g_source_remove(self->telemetry_timeout_id);
        self->telemetry_timeout_id = 0;
    }

    g_clear_object(&self->telemetry_store);
    g_clear_object(&self->settings);

    G_OBJECT_CLASS(hci_window_parent_class)->dispose(object);
}

static void
hci_window_class_init(HciWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    object_class->dispose = hci_window_dispose;

    /* Ensure custom HciGauge GType is registered before building template */
    g_type_ensure(HCI_TYPE_GAUGE);

    gtk_widget_class_set_template_from_resource(
        widget_class,
        "/org/gnome/Hci/ui/window.ui"
    );

    gtk_widget_class_bind_template_child(widget_class, HciWindow, split_view);
    gtk_widget_class_bind_template_child(widget_class, HciWindow, cpu_gauge);
    gtk_widget_class_bind_template_child(widget_class, HciWindow, latency_switch);
    gtk_widget_class_bind_template_child(widget_class, HciWindow, fan_boost_switch);
    gtk_widget_class_bind_template_child(widget_class, HciWindow, telemetry_view);
}

static void
hci_window_init(HciWindow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));

    setup_column_view(self);

    GSettingsSchemaSource *source = g_settings_schema_source_get_default();
    if (source) {
        g_autoptr(GSettingsSchema) schema = g_settings_schema_source_lookup(source, "org.gnome.Hci", TRUE);
        if (schema) {
            self->settings = g_settings_new("org.gnome.Hci");
            g_settings_bind(
                self->settings, "latency-performance-mode",
                self->latency_switch, "active",
                G_SETTINGS_BIND_DEFAULT
            );
            g_settings_bind(
                self->settings, "fan-boost",
                self->fan_boost_switch, "active",
                G_SETTINGS_BIND_DEFAULT
            );
        }
    }

    self->telemetry_timeout_id = g_timeout_add_seconds(2, on_telemetry_tick, self);
}

HciWindow *
hci_window_new(GtkApplication *app)
{
    return g_object_new(
        HCI_TYPE_WINDOW,
        "application", app,
        NULL
    );
}
