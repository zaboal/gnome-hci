#include "hci-database.h"
#include <glib.h>
#include <glib/gstdio.h>

struct _HciDatabase {
    GObject parent_instance;
    sqlite3 *db;
};

G_DEFINE_FINAL_TYPE(HciDatabase, hci_database, G_TYPE_OBJECT)

static void
hci_database_dispose(GObject *object)
{
    HciDatabase *self = HCI_DATABASE(object);

    if (self->db) {
        sqlite3_close(self->db);
        self->db = NULL;
    }

    G_OBJECT_CLASS(hci_database_parent_class)->dispose(object);
}

static void
hci_database_init(HciDatabase *self)
{
    const char *data_dir = g_get_user_data_dir();
    g_autofree char *app_dir = g_build_filename(data_dir, "gnome-hci", NULL);
    g_mkdir_with_parents(app_dir, 0755);

    g_autofree char *db_path = g_build_filename(app_dir, "telemetry.db", NULL);
    int rc = sqlite3_open(db_path, &self->db);
    if (rc != SQLITE_OK) {
        g_warning("Failed to open telemetry database %s: %s", db_path, sqlite3_errmsg(self->db));
        if (self->db) {
            sqlite3_close(self->db);
            self->db = NULL;
        }
        return;
    }

    const char *schema_sql =
        "CREATE TABLE IF NOT EXISTS telemetry_samples ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  timestamp INTEGER NOT NULL,"
        "  metric TEXT NOT NULL,"
        "  value REAL NOT NULL,"
        "  unit TEXT"
        ");";

    char *err_msg = NULL;
    rc = sqlite3_exec(self->db, schema_sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        g_warning("Failed to initialize database schema: %s", err_msg);
        sqlite3_free(err_msg);
    }
}

static void
hci_database_class_init(HciDatabaseClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->dispose = hci_database_dispose;
}

HciDatabase *
hci_database_new(void)
{
    return g_object_new(HCI_TYPE_DATABASE, NULL);
}

gboolean
hci_database_record_metric(HciDatabase *self,
                           const char  *metric_name,
                           double       metric_value,
                           const char  *unit)
{
    g_return_val_if_fail(HCI_IS_DATABASE(self), FALSE);
    g_return_val_if_fail(metric_name != NULL, FALSE);

    if (!self->db)
        return FALSE;

    const char *sql = "INSERT INTO telemetry_samples (timestamp, metric, value, unit) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt = NULL;

    int rc = sqlite3_prepare_v2(self->db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        g_warning("Failed to prepare statement: %s", sqlite3_errmsg(self->db));
        return FALSE;
    }

    gint64 now_unix = g_get_real_time() / G_USEC_PER_SEC;
    sqlite3_bind_int64(stmt, 1, now_unix);
    sqlite3_bind_text(stmt, 2, metric_name, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, metric_value);
    sqlite3_bind_text(stmt, 4, unit ? unit : "", -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}
