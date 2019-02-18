#include <glib.h>
#include <glib/gprintf.h>

#include "settings.h"

GKeyFile *settings_init(void)
{
    gboolean success;
    GError *error = NULL;
    GKeyFile *keyfile = g_key_file_new();

    success = g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error);

    if(!success)
    {
        g_fprintf(stderr, "Error opening config file: %s (%d)\n", error->message, error->code);
        g_key_file_unref(keyfile);
        return NULL;
    }

    return keyfile;
}

gboolean settings_deinit(GKeyFile *keyfile)
{
    GError *error = NULL;
    gboolean success = g_key_file_save_to_file(keyfile, CONFIG_FILE, &error);

    if(!success)
        g_fprintf(stderr, "Error saving config file: %s (%d)\n", error->message, error->code);

    g_key_file_unref(keyfile);

    return success;
}
