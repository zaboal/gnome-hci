#include "hci-gauge.h"
#include <cairo.h>
#include <math.h>

struct _HciGauge {
    GtkWidget parent_instance;
    double fraction;
    char *label;
};

enum {
    PROP_0,
    PROP_FRACTION,
    PROP_LABEL,
    N_PROPS
};

static GParamSpec *properties[N_PROPS] = { NULL };

G_DEFINE_FINAL_TYPE(HciGauge, hci_gauge, GTK_TYPE_WIDGET)

static void
hci_gauge_snapshot(GtkWidget *widget, GtkSnapshot *snapshot)
{
    HciGauge *self = HCI_GAUGE(widget);
    int width = gtk_widget_get_width(widget);
    int height = gtk_widget_get_height(widget);

    if (width <= 0 || height <= 0)
        return;

    graphene_rect_t bounds = GRAPHENE_RECT_INIT(0, 0, (float)width, (float)height);

    cairo_t *cr = gtk_snapshot_append_cairo(snapshot, &bounds);
    if (!cr)
        return;

    double center_x = width * 0.5;
    double center_y = height * 0.5;
    double radius = (width < height ? width : height) * 0.38;

    double start_angle = G_PI * 0.75;
    double end_angle = G_PI * 2.25;
    double sweep = end_angle - start_angle;

    /* Background gauge track */
    cairo_set_line_width(cr, 10.0);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_arc(cr, center_x, center_y, radius, start_angle, end_angle);
    cairo_set_source_rgba(cr, 0.4, 0.4, 0.4, 0.3);
    cairo_stroke(cr);

    /* Active progress arc */
    if (self->fraction > 0.001) {
        double active_end = start_angle + sweep * CLAMP(self->fraction, 0.0, 1.0);
        cairo_arc(cr, center_x, center_y, radius, start_angle, active_end);
        cairo_set_source_rgba(cr, 0.21, 0.52, 0.89, 0.95);
        cairo_stroke(cr);
    }

    /* Text display */
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 18.0);
    cairo_set_source_rgba(cr, 0.9, 0.9, 0.9, 0.95);

    char val_buf[32];
    snprintf(val_buf, sizeof(val_buf), "%.2f IPC", self->fraction * 2.5);

    cairo_text_extents_t extents;
    cairo_text_extents(cr, val_buf, &extents);
    cairo_move_to(cr, center_x - (extents.width * 0.5), center_y + (extents.height * 0.35));
    cairo_show_text(cr, val_buf);

    if (self->label && *self->label) {
        cairo_set_font_size(cr, 11.0);
        cairo_set_source_rgba(cr, 0.7, 0.7, 0.7, 0.8);
        cairo_text_extents(cr, self->label, &extents);
        cairo_move_to(cr, center_x - (extents.width * 0.5), center_y + radius * 0.65);
        cairo_show_text(cr, self->label);
    }

    cairo_destroy(cr);
}

static void
hci_gauge_measure(GtkWidget      *widget,
                  GtkOrientation  orientation,
                  int             for_size,
                  int            *minimum,
                  int            *natural,
                  int            *minimum_baseline,
                  int            *natural_baseline)
{
    (void)widget;
    (void)orientation;
    (void)for_size;
    (void)minimum_baseline;
    (void)natural_baseline;

    if (minimum)
        *minimum = 120;
    if (natural)
        *natural = 160;
}

static void
hci_gauge_get_property(GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
    HciGauge *self = HCI_GAUGE(object);

    switch (prop_id) {
    case PROP_FRACTION:
        g_value_set_double(value, self->fraction);
        break;
    case PROP_LABEL:
        g_value_set_string(value, self->label);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hci_gauge_set_property(GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
    HciGauge *self = HCI_GAUGE(object);

    switch (prop_id) {
    case PROP_FRACTION:
        hci_gauge_set_fraction(self, g_value_get_double(value));
        break;
    case PROP_LABEL:
        hci_gauge_set_label(self, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hci_gauge_finalize(GObject *object)
{
    HciGauge *self = HCI_GAUGE(object);

    g_free(self->label);

    G_OBJECT_CLASS(hci_gauge_parent_class)->finalize(object);
}

static void
hci_gauge_class_init(HciGaugeClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    object_class->get_property = hci_gauge_get_property;
    object_class->set_property = hci_gauge_set_property;
    object_class->finalize = hci_gauge_finalize;

    widget_class->snapshot = hci_gauge_snapshot;
    widget_class->measure = hci_gauge_measure;

    gtk_widget_class_set_css_name(widget_class, "telemetry-gauge");

    properties[PROP_FRACTION] = g_param_spec_double(
        "fraction",
        "Fraction",
        "Normalized metric value between 0.0 and 1.0",
        0.0, 1.0, 0.0,
        G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
    );

    properties[PROP_LABEL] = g_param_spec_string(
        "label",
        "Label",
        "Label beneath gauge readout",
        "Metric",
        G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(object_class, N_PROPS, properties);
}

static void
hci_gauge_init(HciGauge *self)
{
    self->fraction = 0.0;
    self->label = g_strdup("Metric");
}

GtkWidget *
hci_gauge_new(void)
{
    return g_object_new(HCI_TYPE_GAUGE, NULL);
}

void
hci_gauge_set_fraction(HciGauge *self, double fraction)
{
    g_return_if_fail(HCI_IS_GAUGE(self));

    fraction = CLAMP(fraction, 0.0, 1.0);
    if (fabs(self->fraction - fraction) > 0.0001) {
        self->fraction = fraction;
        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_FRACTION]);
        gtk_widget_queue_draw(GTK_WIDGET(self));
    }
}

double
hci_gauge_get_fraction(HciGauge *self)
{
    g_return_val_if_fail(HCI_IS_GAUGE(self), 0.0);
    return self->fraction;
}

void
hci_gauge_set_label(HciGauge *self, const char *label)
{
    g_return_if_fail(HCI_IS_GAUGE(self));

    if (g_strcmp0(self->label, label) != 0) {
        g_free(self->label);
        self->label = g_strdup(label ? label : "");
        g_object_notify_by_pspec(G_OBJECT(self), properties[PROP_LABEL]);
        gtk_widget_queue_draw(GTK_WIDGET(self));
    }
}

const char *
hci_gauge_get_label(HciGauge *self)
{
    g_return_val_if_fail(HCI_IS_GAUGE(self), "");
    return self->label;
}
