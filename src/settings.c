#include "settings.h"

static void cb_sounds(GtkWidget *widget, gboolean state, gpointer data)
{
    memory_t *memory = data;
    g_key_file_set_boolean(memory->keyfile, "Game", "sounds", state);
}

static void cb_pairs(GtkSpinButton *spin_button, gpointer data)
{
    memory_t *memory = data;
    gint pairs = gtk_spin_button_get_value_as_int(spin_button);
    g_key_file_set_integer(memory->keyfile, "Game", "pairs", pairs);
}

static void cb_combo_box_set(GtkComboBox *widget, gpointer data)
{
    memory_t *memory = data;
    GtkComboBox *combo_box = widget;

    gint value = gtk_combo_box_get_active(combo_box);
    g_key_file_set_integer (memory->keyfile, "Game", "active_set", value);
}

static void cb_combo_box_cover(GtkComboBox *widget, gpointer data)
{
    memory_t *memory = data;
    GtkComboBox *combo_box = widget;

    gint value = gtk_combo_box_get_active(combo_box);
    g_key_file_set_integer (memory->keyfile, "Game", "active_cover", value);
}

// Function to open a dialog box with a message
void settings_dialog(memory_t *memory)
{
    GKeyFile *keyfile = memory->keyfile;

    GtkDialogFlags flags = 0;
    // Create the widgets
    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Einstellungen",
                                       GTK_WINDOW(memory->window),
                                       flags,
                                       "_OK",
                                       GTK_RESPONSE_NONE,
                                       NULL);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Ensure that the dialog box is destroyed when the user responds
    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 20);

    GtkWidget *label_sound = gtk_label_new("Ton");
    gtk_widget_set_halign(label_sound, GTK_ALIGN_END);
    GtkWidget *switch_sound = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(switch_sound), g_key_file_get_boolean(keyfile, "Game", "sounds", NULL));
    gtk_widget_set_halign(switch_sound, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(switch_sound, GTK_ALIGN_CENTER);
    g_signal_connect(switch_sound, "state-set", G_CALLBACK(cb_sounds), memory);
    gtk_grid_attach(GTK_GRID(grid), label_sound, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), switch_sound, 1, 0, 1, 1);

    GtkWidget *label_pairs = gtk_label_new("Paare");
    gtk_widget_set_halign(label_pairs, GTK_ALIGN_END);
    GtkWidget *spin_pairs = gtk_spin_button_new_with_range(8, 32, 1);
    gtk_widget_set_halign(spin_pairs, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(spin_pairs, GTK_ALIGN_CENTER);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_pairs), g_key_file_get_integer(keyfile, "Game", "pairs", NULL));
    g_signal_connect(spin_pairs, "value-changed", G_CALLBACK(cb_pairs), memory);
    gtk_grid_attach(GTK_GRID(grid), label_pairs, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), spin_pairs, 1, 1, 1, 1);

    GtkWidget *label_set = gtk_label_new("Kartendeck");
    gtk_widget_set_halign(label_set, GTK_ALIGN_END);
    GtkWidget *combo_box_set = gtk_combo_box_text_new();
    for(gsize i = 0; memory->list_sets[i].title != NULL; i++)
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box_set), memory->list_sets[i].title);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box_set), g_key_file_get_integer(memory->keyfile, "Game", "active_set", NULL));
    g_signal_connect(combo_box_set, "changed", G_CALLBACK(cb_combo_box_set), memory);
    gtk_grid_attach(GTK_GRID(grid), label_set, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_box_set, 1, 2, 1, 1);

    GtkWidget *label_cover = gtk_label_new("Deckblatt");
    gtk_widget_set_halign(label_cover, GTK_ALIGN_END);
    GtkWidget *combo_box_cover = gtk_combo_box_text_new();
    for(gsize i = 0; memory->list_covers[i].title != NULL; i++)
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box_cover), memory->list_covers[i].title);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo_box_cover), g_key_file_get_integer(memory->keyfile, "Game", "active_cover", NULL));
    g_signal_connect(combo_box_cover, "changed", G_CALLBACK(cb_combo_box_cover), memory);
    gtk_grid_attach(GTK_GRID(grid), label_cover, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_box_cover, 1, 3, 1, 1);

    gtk_container_add(GTK_CONTAINER(content_area), grid);

    gtk_widget_show_all(dialog);
}
