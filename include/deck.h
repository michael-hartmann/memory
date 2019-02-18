#ifndef __DECK_H
#define __DECK_H

    #include "card.h"

    typedef struct {
        gint elements;
        card_t **cards;
    } deck_t;
    
    deck_t *deck_new(const gchar *directory, const gchar *cover, gint pairs);
    void deck_free(deck_t *deck);

#endif

