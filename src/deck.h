#ifndef DECK_H
#define DECK_H

#include <gtk/gtk.h>

#include "card.h"

typedef struct {
    void *memory;
    gint clicks;
    gint unsolved;
    gint cards_open;
    gint pairs;
    card_t *cards_shown[2];
    card_t **cards;
    GDateTime *start_time;
} deck_t;

deck_t *deck_new(void *memory, const gchar *directory, const gchar *cover, gint pairs);
gboolean deck_click_on_card(deck_t *deck, card_t *card);
GTimeSpan deck_get_elapsed_time(deck_t *deck);
gboolean deck_is_solved(deck_t *deck);
gint deck_get_points(deck_t *deck);
void deck_free(deck_t *deck);

#endif

