#include "config.h"
#include "hci-application.h"
#include "hci-window.h"
#include "hci-preferences-window.h"
#include "hci-database.h"

struct _HciApplication {
    AdwApplication parent_instance;
    HciDatabase *db;
};

G_DEFINE_FINAL_TYPE(HciApplication, hci_application, ADW_TYPE_APPLICATION)

static void
on_action_preferences(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;
    (void)parameter;

    HciApplication *self = HCI_APPLICATION(user_data);
    GtkWindow *active_window = gtk_application_get_active_window(GTK_APPLICATION(self));

    HciPreferencesWindow *prefs = hci_preferences_window_new(active_window);
    adw_dialog_present(ADW_DIALOG(prefs), active_window ? GTK_WIDGET(active_window) : NULL);
}

static void
on_action_about(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;
    (void)parameter;

    HciApplication *self = HCI_APPLICATION(user_data);
    GtkWindow *active_window = gtk_application_get_active_window(GTK_APPLICATION(self));

    const char *developers[] = {
        "Bogdan Zažigin <git+me@zba.su>",
        NULL
    };

    adw_show_about_dialog(
        GTK_WIDGET(active_window),
        "application-name", "GNOME HCI",
        "application-icon", APP_ID,
        "developer-name", "Bogdan Zažigin",
        "version", PACKAGE_VERSION,
        "copyright", "© 2026 Bogdan Zažigin",
        "license-type", GTK_LICENSE_GPL_3_0,
        "developers", developers,
        "website", "https://github.com/zaboal/gnome-hci",
        "issue-url", "https://github.com/zaboal/gnome-hci/issues",
        NULL
    );
}

static void
on_action_quit(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    (void)action;
    (void)parameter;

    HciApplication *self = HCI_APPLICATION(user_data);
    g_application_quit(G_APPLICATION(self));
}

static const GActionEntry app_actions[] = {
    { "preferences", on_action_preferences, NULL, NULL, NULL, { 0, 0, 0 } },
    { "about", on_action_about, NULL, NULL, NULL, { 0, 0, 0 } },
    { "quit", on_action_quit, NULL, NULL, NULL, { 0, 0, 0 } },
};

static void
hci_application_init(HciApplication *self)
{
    self->db = hci_database_new();
}

static void
hci_application_startup(GApplication *app)
{
    G_APPLICATION_CLASS(hci_application_parent_class)->startup(app);

    g_action_map_add_action_entries(
        G_ACTION_MAP(app),
        app_actions,
        G_N_ELEMENTS(app_actions),
        app
    );

    const char *accels_quit[] = { "<primary>q", NULL };
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.quit", accels_quit);

    const char *accels_pref[] = { "<primary>comma", NULL };
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.preferences", accels_pref);

    /* Load custom styling from GResource */
    g_autoptr(GtkCssProvider) provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(provider, "/org/gnome/Hci/style.css");
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

static void
hci_application_activate(GApplication *app)
{
    GtkWindow *window = gtk_application_get_active_window(GTK_APPLICATION(app));

    if (!window) {
        window = GTK_WINDOW(hci_window_new(GTK_APPLICATION(app)));
    }

    gtk_window_present(window);
}

static void
hci_application_dispose(GObject *object)
{
    HciApplication *self = HCI_APPLICATION(object);

    g_clear_object(&self->db);

    G_OBJECT_CLASS(hci_application_parent_class)->dispose(object);
}

static void
hci_application_class_init(HciApplicationClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GApplicationClass *app_class = G_APPLICATION_CLASS(klass);

    object_class->dispose = hci_application_dispose;
    app_class->startup = hci_application_startup;
    app_class->activate = hci_application_activate;
}

HciApplication *
hci_application_new(const char *application_id, GApplicationFlags flags)
{
    return g_object_new(
        HCI_TYPE_APPLICATION,
        "application-id", application_id,
        "flags", flags,
        NULL
    );
}
