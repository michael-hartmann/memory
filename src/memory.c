#include <math.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib/gstdio.h>
#include <gtk/gtk.h>

#include "card.h"
#include "deck.h"
#include "settings.h"
#include "memory.h"

state_t state;
GKeyFile *keyfile;

static void cb_new(GtkWidget *widget, gpointer data)
{
    gint i = 0;
    gint pairs     = g_key_file_get_integer(keyfile, "Game", "pairs", NULL);
    gchar *set     = g_key_file_get_string (keyfile, "Game", "set",   NULL);
    gchar *cover   = g_key_file_get_string (keyfile, "Game", "cover", NULL);
    gchar *ratio_s = g_key_file_get_string (keyfile, "Game", "ratio", NULL);
    gchar **list   = g_strsplit(ratio_s, ":", 2);
    gint64 xratio  = g_ascii_strtoll(list[0], NULL, 10);
    gint64 yratio  = g_ascii_strtoll(list[1], NULL, 10);
    gdouble ratio  = (gdouble)xratio/yratio;

    gint rows = ceil(sqrt(   ratio * 2*pairs));
    gint cols = ceil(sqrt(1./ratio * 2*pairs));

    g_printf("rows=%d, cols=%d, pairs=%d\n", rows, cols, pairs);

    if(state.grid != NULL)
        gtk_widget_destroy(state.grid);
    if(state.deck != NULL)
        deck_free(state.deck);
    if(state.start != NULL)
        g_date_time_unref(state.start);

    state.clicks = 0;
    state.cards_open = 0;
    state.cards_shown[0] = state.cards_shown[1] = NULL;
    state.start = g_date_time_new_now_local();
    state.deck  = deck_new(set, cover, pairs);
    state.unsolved = pairs;

    state.grid = gtk_grid_new();
    for(gint y = 0; y < cols; y++)
        for(gint x = 0; x < rows; x++)
        {
            if(i >= 2*pairs)
                break;

            GtkWidget *button;
            card_t *card = state.deck->cards[i++];
            button = card_get_button(card);

            g_signal_connect(button, "clicked", G_CALLBACK(cb_clicked), card);
            gtk_grid_attach(GTK_GRID(state.grid), button, x, y, 1, 1);
        }

    gtk_box_pack_start(GTK_BOX(state.vbox), state.grid, FALSE, FALSE, 5);
    gtk_widget_show_all(state.grid);

    g_free(set);
    g_free(cover);
    g_free(ratio_s);
    g_strfreev(list);
}


static void cb_clicked(GtkWidget *widget, gpointer data)
{
    card_t *card = data;

    state.clicks++;

    if(state.cards_open == 0)
    {
        card_set_shown(card);
        state.cards_shown[0] = card;
        state.cards_open = 1;
    }
    else if(state.cards_open == 1)
    {
        if(state.cards_shown[0] == card)
        {
            card_set_hidden(card);
            state.cards_open = 0;
        }
        else
        {
            if(card_compare(state.cards_shown[0], card) == 0)
            {
                state.cards_open = 0;
                card_set_solved(state.cards_shown[0]);
                card_set_solved(card);
                state.unsolved--;
            }
            else
            {
                card_set_shown(card);
                state.cards_shown[1] = card;
                state.cards_open = 2;
            }
        }
    }
    else
    {
        if(state.cards_shown[0] == card)
        {
            card_set_hidden(card);
            state.cards_shown[0] = state.cards_shown[1];
            state.cards_open = 1;
        }
        else if(state.cards_shown[1] == card)
        {
            card_set_hidden(card);
            state.cards_open = 1;
        }
        else
        {
            card_set_hidden(state.cards_shown[0]);
            card_set_hidden(state.cards_shown[1]);

            card_set_shown(card);
            state.cards_shown[0] = card;
            state.cards_open = 1;
        }
    }

    if(state.unsolved == 0)
    {
        GDateTime *now = g_date_time_new_now_local();
        GTimeSpan diff = g_date_time_difference(now, state.start);
        g_date_time_unref(state.start);
        g_date_time_unref(now);
        state.start = NULL;
        g_printf("fertig, clicks %d, deltat: %.1lf\n", state.clicks, diff/1e6);
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

int main(int argc, char *argv[])
{
    GtkWidget *window, *menu, *vbox;
    GtkWidget *menu_new, *menu_quit, *menubar, *menuitem;
    gchar *path, *title;

    /* init gtk */
    gtk_init(&argc, &argv);

    /* change to directory */
    path = g_path_get_dirname(argv[0]);
    g_chdir(path);
    g_free(path);

    keyfile = settings_init();
    if(keyfile == NULL)
        return 1;

    title = g_key_file_get_string(keyfile, "Game", "title", NULL);

    /* create main window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title);

    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(cb_quit), NULL);

    gtk_container_set_border_width(GTK_CONTAINER (window), 1);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    /* menu */
    menu = gtk_menu_new();
    
    menu_new = gtk_menu_item_new_with_label("Neu");
    g_signal_connect(menu_new, "activate", G_CALLBACK(cb_new), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_new);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

    menu_quit = gtk_menu_item_new_with_label("Beenden");
    g_signal_connect(menu_quit, "activate", G_CALLBACK(cb_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_quit);

    menubar = gtk_menu_bar_new();
    menuitem = gtk_menu_item_new_with_label("Spiel");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 5);

    /* initialize state */
    state.grid = NULL;
    state.vbox = vbox;
    state.deck = NULL;
    state.start = NULL;

    /* create new game */
    cb_new(NULL, NULL);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);

    gtk_main();

    settings_deinit(keyfile);

    g_free(title);

    return 0;
}
