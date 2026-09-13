#include "config.h"
#include "hci-application.h"

int
main(int argc, char *argv[])
{
    g_autoptr(HciApplication) app = NULL;

    app = hci_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);

    return g_application_run(G_APPLICATION(app), argc, argv);
}
