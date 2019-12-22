#ifndef MEMORY_H
#define MEMORY_H

#include <gtk/gtk.h>
#include "card.h"

    typedef struct {
        int clicks;
        int unsolved;
        int cards_open;
        int pairs;
        card_t *cards_shown[2];
        GtkWidget *window, *vbox, *scrolled_window;
        deck_t *deck;
        GDateTime *start;
    } state_t;


    static void cb_clicked(GtkWidget *widget, gpointer data);
    static void cb_new(GtkWidget *widget, gpointer data);
    static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
    static void cb_quit(GtkWidget *widget, gpointer data);

#endif
