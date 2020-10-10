#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "card.h"
#include "deck.h"
#include "memory.h"
#include "settings.h"

#define CONFIG_FILE "settings.cfg"

static void cb_clicked(GtkWidget *widget, gpointer data);
static void cb_new(GtkWidget *widget, gpointer data);
static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
static void cb_quit(GtkWidget *widget, gpointer data);
static gboolean timer(gpointer data);

static void cb_new(GtkWidget *widget, gpointer data)
{
    memory_t *memory = data;

    gint pairs   = g_key_file_get_integer(memory->keyfile, "Game", "pairs", NULL);
    gchar *set   = g_key_file_get_string (memory->keyfile, "Game", "set",   NULL);
    gchar *cover = g_key_file_get_string (memory->keyfile, "Game", "cover", NULL);

    if(memory->vbox != NULL)
        gtk_container_remove(GTK_CONTAINER(memory->scrolled_window), memory->vbox);

    deck_free(memory->deck);
    memory->deck = deck_new(memory, set, cover, pairs);

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget* flowbox = gtk_flow_box_new();
    for(gint i = 0; i < 2*pairs; i++)
    {
        card_t *card = memory->deck->cards[i];
        GtkWidget *button = card_get_button(card);

        g_signal_connect(button, "clicked", G_CALLBACK(cb_clicked), card);
        gtk_flow_box_insert(GTK_FLOW_BOX(flowbox), button, i);
    }
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(flowbox), 5);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flowbox), 24);

    gtk_box_pack_start(GTK_BOX(vbox), flowbox, FALSE, FALSE, 5);
    gtk_widget_show_all(vbox);

    gtk_container_add(GTK_CONTAINER(memory->scrolled_window),vbox);

    memory->vbox = vbox;

    g_timeout_add(0, timer, memory);

    g_free(set);
    g_free(cover);
}

static void cb_settings(GtkWidget *widget, gpointer data)
{
    memory_t *memory = data;
    settings_dialog(memory);
}

static void cb_clicked(GtkWidget *widget, gpointer data)
{
    card_t *card = data;
    deck_t *deck = card->deck;
    memory_t *memory = deck->memory;

    gboolean solved = deck_click_on_card(deck, card);
    if(solved)
    {
        GTimeSpan timespan = deck_get_elapsed_time(deck);
        gint minutes = timespan/60e6;
        gint seconds = (gint)(timespan/1e6) % 60;

        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(memory->window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_YES_NO,
            "Geschafft!\n\nDu hast %d:%02d Zeit und %d Versuche gebraucht um alle %d Paare zu finden.\n\nNeues Spiel beginnen?", minutes, seconds, deck->clicks/2, deck->pairs);
        gint answer = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        if(answer == GTK_RESPONSE_YES)
            gtk_menu_item_activate((GtkMenuItem *)memory->menu_new);
    }
}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    return FALSE;
}

static void cb_quit(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

static gboolean timer(gpointer data)
{
    memory_t* memory = data;

    if(memory->deck && !deck_is_solved(memory->deck))
    {
        GTimeSpan elapsed = deck_get_elapsed_time(memory->deck);
        gint seconds = elapsed/1000000;
        glong diff = (seconds+1)*1000-elapsed/1000;

        gchar *text = g_strdup_printf("%d:%02d", seconds/60, seconds%60);
        gtk_label_set_text((GtkLabel *)memory->status, text);
        g_free(text);
        
        g_timeout_add(diff, timer, memory);
    }
    return FALSE;
}

memory_t *memory_create()
{
    memory_t *memory = g_malloc0(sizeof(memory_t));

    memory->keyfile = g_key_file_new();
    if(!g_key_file_load_from_file(memory->keyfile, CONFIG_FILE, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, NULL))
    {
        memory_destroy(memory);
        return NULL;
    }

    /* create main window */
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gchar *title = g_key_file_get_string(memory->keyfile, "Game", "title", NULL);
    gtk_window_set_title(GTK_WINDOW(window), title);
    g_free(title);
    gtk_widget_set_size_request(window, 800, 600);

    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(cb_quit), NULL);

    gtk_container_set_border_width(GTK_CONTAINER (window), 1);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    /* menu */
    GtkWidget *menu = gtk_menu_new();

    /* menu -> new */
    {
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *icon = gtk_image_new_from_icon_name("document-new", GTK_ICON_SIZE_MENU);
        GtkWidget *label = gtk_label_new("Neues Spiel");
        GtkWidget *menu_new = gtk_menu_item_new();

        gtk_container_add(GTK_CONTAINER(box), icon);
        gtk_container_add(GTK_CONTAINER(box), label);
        gtk_container_add(GTK_CONTAINER(menu_new), box);

        gtk_widget_show_all(menu_new);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_new);
        g_signal_connect(menu_new, "activate", G_CALLBACK(cb_new), memory);

        memory->menu_new = menu_new;
    }

    /* separator */
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

    /* menu -> settings */
    {
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *label = gtk_label_new("Einstellungen");
        GtkWidget *menu_settings = gtk_menu_item_new();

        gtk_container_add(GTK_CONTAINER(box), label);
        gtk_container_add(GTK_CONTAINER(menu_settings), box);

        gtk_widget_show_all(menu_settings);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_settings);
        g_signal_connect(menu_settings, "activate", G_CALLBACK(cb_settings), memory);
    }

    /* separator */
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

    /* menu -> quit */
    {
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *icon = gtk_image_new_from_icon_name("application-exit", GTK_ICON_SIZE_MENU);
        GtkWidget *label = gtk_label_new("Beenden");
        GtkWidget *menu_quit = gtk_menu_item_new();

        gtk_container_add(GTK_CONTAINER(box), icon);
        gtk_container_add(GTK_CONTAINER(box), label);

        gtk_container_add(GTK_CONTAINER(menu_quit), box);

        gtk_widget_show_all(menu_quit);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_quit);
        g_signal_connect(menu_quit, "activate", G_CALLBACK(cb_quit), NULL);
    }

    GtkWidget *menubar = gtk_menu_bar_new();
    GtkWidget *menuitem = gtk_menu_item_new_with_label("Spiel");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

    /* status label */
    GtkWidget *status = gtk_label_new("");
    gtk_label_set_xalign((GtkLabel *)status, 0);
    gtk_box_pack_start(GTK_BOX(vbox), status, FALSE, FALSE, 5);

    /* initialize state */
    memory->vbox = NULL;
    memory->window = window;
    memory->scrolled_window = scrolled_window;
    memory->status = status;

    /* create new game */
    gtk_menu_item_activate((GtkMenuItem *)memory->menu_new);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);

    return memory;
}

void memory_destroy(memory_t *memory)
{
    if(memory)
    {
        if(memory->keyfile)
        {
            g_key_file_save_to_file(memory->keyfile, CONFIG_FILE, NULL);
            g_key_file_unref(memory->keyfile);
        }
        g_free(memory);
    }
}

int main(int argc, char *argv[])
{
    /* init gtk */
    gtk_init(&argc, &argv);

    /* change to directory */
    gchar *path = g_path_get_dirname(argv[0]);
    g_chdir(path);
    g_free(path);

    memory_t *memory = memory_create();

    gtk_main();

    memory_destroy(memory);

    return 0;
}
