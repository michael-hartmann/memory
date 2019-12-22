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
    gint pairs   = g_key_file_get_integer(keyfile, "Game", "pairs", NULL);
    gchar *set   = g_key_file_get_string (keyfile, "Game", "set",   NULL);
    gchar *cover = g_key_file_get_string (keyfile, "Game", "cover", NULL);

    g_printf("pairs=%d\n", pairs);

    if(state.vbox != NULL)
        gtk_container_remove(GTK_CONTAINER(state.scrolled_window), state.vbox);
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

    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget* flowbox = gtk_flow_box_new();
    for(gint i = 0; i < 2*pairs; i++)
    {
        card_t *card = state.deck->cards[i];
        GtkWidget *button = card_get_button(card);

        g_signal_connect(button, "clicked", G_CALLBACK(cb_clicked), card);
        gtk_flow_box_insert(GTK_FLOW_BOX(flowbox), button, i);
    }
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(flowbox), 5);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flowbox), 24);

    gtk_box_pack_start(GTK_BOX(vbox), flowbox, FALSE, FALSE, 5);
    gtk_widget_show_all(vbox);

    gtk_container_add(GTK_CONTAINER(state.scrolled_window),vbox);

    state.vbox = vbox;

    g_free(set);
    g_free(cover);
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
    /* init gtk */
    gtk_init(&argc, &argv);

    /* change to directory */
    gchar *path = g_path_get_dirname(argv[0]);
    g_chdir(path);
    g_free(path);

    keyfile = settings_init();
    if(keyfile == NULL)
        return 1;

    gchar *title = g_key_file_get_string(keyfile, "Game", "title", NULL);

    /* create main window */
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title);
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

		gtk_container_add(GTK_CONTAINER (menu_new), box);

		gtk_widget_show_all(menu_new);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_new);
        g_signal_connect(menu_new, "activate", G_CALLBACK(cb_new), NULL);
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

		gtk_container_add(GTK_CONTAINER (menu_quit), box);

		gtk_widget_show_all(menu_quit);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_quit);
        g_signal_connect(menu_quit, "activate", G_CALLBACK(cb_quit), NULL);
    }

    GtkWidget *menubar = gtk_menu_bar_new();
    GtkWidget *menuitem = gtk_menu_item_new_with_label("Spiel");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menuitem);

    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);

    /* initialize state */
    //state.grid = vbox2;
    state.vbox = NULL;
    state.scrolled_window = scrolled_window;
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
