#include "hci-telemetry-model.h"

struct _HciTelemetryItem {
    GObject parent_instance;
    char *name;
    char *value;
    char *unit;
};

enum {
    PROP_0,
    PROP_NAME,
    PROP_VALUE,
    PROP_UNIT,
    N_PROPS
};

static GParamSpec *properties[N_PROPS] = { NULL };

G_DEFINE_FINAL_TYPE(HciTelemetryItem, hci_telemetry_item, G_TYPE_OBJECT)

static void
hci_telemetry_item_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    HciTelemetryItem *self = HCI_TELEMETRY_ITEM(object);

    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string(value, self->name);
        break;
    case PROP_VALUE:
        g_value_set_string(value, self->value);
        break;
    case PROP_UNIT:
        g_value_set_string(value, self->unit);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hci_telemetry_item_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    HciTelemetryItem *self = HCI_TELEMETRY_ITEM(object);

    switch (prop_id) {
    case PROP_NAME:
        hci_telemetry_item_set_name(self, g_value_get_string(value));
        break;
    case PROP_VALUE:
        hci_telemetry_item_set_value(self, g_value_get_string(value));
        break;
    case PROP_UNIT:
        hci_telemetry_item_set_unit(self, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hci_telemetry_item_finalize(GObject *object)
{
    HciTelemetryItem *self = HCI_TELEMETRY_ITEM(object);

    g_free(self->name);
    g_free(self->value);
    g_free(self->unit);

    G_OBJECT_CLASS(hci_telemetry_item_parent_class)->finalize(object);
}

static void
hci_telemetry_item_class_init(HciTelemetryItemClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    object_class->get_property = hci_telemetry_item_get_property;
    object_class->set_property = hci_telemetry_item_set_property;
    object_class->finalize = hci_telemetry_item_finalize;

    properties[PROP_NAME] = g_param_spec_string(
        "name", "Name", "Metric Name",
        "", G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
    );

    properties[PROP_VALUE] = g_param_spec_string(
        "value", "Value", "Metric Value",
        "", G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
    );

    properties[PROP_UNIT] = g_param_spec_string(
        "unit", "Unit", "Metric Unit",
        "", G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(object_class, N_PROPS, properties);
}

static void
hci_telemetry_item_init(HciTelemetryItem *self)
{
    self->name = NULL;
    self->value = NULL;
    self->unit = NULL;
}

HciTelemetryItem *
hci_telemetry_item_new(const char *name, const char *value, const char *unit)
{
    return g_object_new(
        HCI_TYPE_TELEMETRY_ITEM,
        "name", name,
        "value", value,
        "unit", unit,
        NULL
    );
}

const char *
hci_telemetry_item_get_name(HciTelemetryItem *self)
{
    g_return_val_if_fail(HCI_IS_TELEMETRY_ITEM(self), "");
    return self->name ? self->name : "";
}

void
hci_telemetry_item_set_name(HciTelemetryItem *self, const char *name)
{
    g_return_if_fail(HCI_IS_TELEMETRY_ITEM(self));

    if (g_strcmp0(self->name, name) != 0) {
        g_free(self->name);
        self->name = g_strdup(name);
        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_NAME]);
    }
}

const char *
hci_telemetry_item_get_value(HciTelemetryItem *self)
{
    g_return_val_if_fail(HCI_IS_TELEMETRY_ITEM(self), "");
    return self->value ? self->value : "";
}

void
hci_telemetry_item_set_value(HciTelemetryItem *self, const char *value)
{
    g_return_if_fail(HCI_IS_TELEMETRY_ITEM(self));

    if (g_strcmp0(self->value, value) != 0) {
        g_free(self->value);
        self->value = g_strdup(value);
        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_VALUE]);
    }
}

const char *
hci_telemetry_item_get_unit(HciTelemetryItem *self)
{
    g_return_val_if_fail(HCI_IS_TELEMETRY_ITEM(self), "");
    return self->unit ? self->unit : "";
}

void
hci_telemetry_item_set_unit(HciTelemetryItem *self, const char *unit)
{
    g_return_if_fail(HCI_IS_TELEMETRY_ITEM(self));

    if (g_strcmp0(self->unit, unit) != 0) {
        g_free(self->unit);
        self->unit = g_strdup(unit);
        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_UNIT]);
    }
}

GListStore *
hci_telemetry_model_create_default(void)
{
    GListStore *store = g_list_store_new(HCI_TYPE_TELEMETRY_ITEM);

    g_autoptr(HciTelemetryItem) item1 = hci_telemetry_item_new("CPU0 Frequency", "3300", "MHz");
    g_autoptr(HciTelemetryItem) item2 = hci_telemetry_item_new("CPU1 Frequency", "3300", "MHz");
    g_autoptr(HciTelemetryItem) item3 = hci_telemetry_item_new("Package Temperature", "58.0", "°C");
    g_autoptr(HciTelemetryItem) item4 = hci_telemetry_item_new("Fan Acoustic Speed", "4520", "RPM");
    g_autoptr(HciTelemetryItem) item5 = hci_telemetry_item_new("Microarchitectural IPC", "1.88", "ins/cyc");
    g_autoptr(HciTelemetryItem) item6 = hci_telemetry_item_new("BORE Scheduler Latency", "1.0", "ms");
    g_autoptr(HciTelemetryItem) item7 = hci_telemetry_item_new("AC Mains Power Link", "Online", "AC");

    g_list_store_append(store, item1);
    g_list_store_append(store, item2);
    g_list_store_append(store, item3);
    g_list_store_append(store, item4);
    g_list_store_append(store, item5);
    g_list_store_append(store, item6);
    g_list_store_append(store, item7);

    return store;
}
