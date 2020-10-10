#include <gtk/gtk.h>
#include <glib/gprintf.h>

#include "deck.h"


/* see http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle#The_modern_algorithm */
static void shuffle(void **list, size_t elements)
{
    GRand *rand = g_rand_new();

    for(gint i = elements-1; i >= 1; i--)
    {
        void *temp;
        gint j = g_rand_int_range(rand, 0, i+1);

        temp    = list[j];
        list[j] = list[i];
        list[i] = temp;
    }

    g_rand_free(rand);
}

deck_t *deck_new(void *memory, const gchar *directory, const gchar *cover, gint pairs)
{
    gint elems = 0;
    const gchar *elem;
    gchar **list = NULL;
    GError *error = NULL;
    deck_t *deck = NULL;
    GDir *dir = g_dir_open(directory, 0, &error);

    if(error) 
    {
        g_fprintf(stderr, "Error reading deck %s (%d)\n", error->message, error->code);
        return NULL;
    }

    while((elem=g_dir_read_name(dir)))
    {
        elems++;
        list = g_realloc(list, elems*sizeof(const gchar *));
        list[elems-1] = g_strconcat(directory, "/", elem, NULL);
    }
    g_dir_close(dir);

    shuffle((void **)list, elems);

    deck = g_malloc(sizeof(deck_t));
    deck->memory     = memory;
    deck->clicks     = 0;
    deck->unsolved   = pairs;
    deck->cards_open = 0;
    deck->pairs      = pairs;
    deck->cards      = g_malloc(2*pairs*sizeof(card_t *));
    deck->start_time = g_date_time_new_now_local();
    deck->cards_shown[0] = deck->cards_shown[1] = NULL;

    for(gint i = 0; i < pairs; i++)
    {
        deck->cards[2*i]   = card_new(deck, list[i], cover);
        deck->cards[2*i+1] = card_new(deck, list[i], cover);
        g_free(list[i]);
    }

    for(gint i = pairs; i < elems; i++)
        g_free(list[i]);
    g_free(list);

    shuffle((void **)deck->cards, 2*pairs);

    return deck;
}

void deck_free(deck_t *deck)
{
    if(deck != NULL)
    {
        for(gint i = 0; i < 2*deck->pairs; i++)
            card_free(deck->cards[i]);

        g_date_time_unref(deck->start_time);

        g_free(deck->cards);
        g_free(deck);
    }
}

gboolean deck_click_on_card(deck_t *deck, card_t *card)
{
    deck->clicks++;

    if(deck->cards_open == 0)
    {
        card_set_shown(card);
        deck->cards_shown[0] = card;
        deck->cards_open = 1;
    }
    else if(deck->cards_open == 1)
    {
        if(deck->cards_shown[0] == card)
        {
            card_set_hidden(card);
            deck->cards_open = 0;
        }
        else
        {
            if(card_compare(deck->cards_shown[0], card) == 0)
            {
                deck->cards_open = 0;
                card_set_solved(deck->cards_shown[0]);
                card_set_solved(card);
                deck->unsolved--;
            }
            else
            {
                card_set_shown(card);
                deck->cards_shown[1] = card;
                deck->cards_open = 2;
            }
        }
    }
    else
    {
        if(deck->cards_shown[0] == card)
        {
            card_set_hidden(card);
            deck->cards_shown[0] = deck->cards_shown[1];
            deck->cards_open = 1;
        }
        else if(deck->cards_shown[1] == card)
        {
            card_set_hidden(card);
            deck->cards_open = 1;
        }
        else
        {
            card_set_hidden(deck->cards_shown[0]);
            card_set_hidden(deck->cards_shown[1]);

            card_set_shown(card);
            deck->cards_shown[0] = card;
            deck->cards_open = 1;
        }
    }

    return deck_is_solved(deck);
}

gboolean deck_is_solved(deck_t *deck)
{
    if(deck->unsolved == 0)
        return TRUE;
    else
        return FALSE;
}

GTimeSpan deck_get_elapsed_time(deck_t *deck)
{
    GDateTime *now = g_date_time_new_now_local();
    GTimeSpan timespan = g_date_time_difference(now, deck->start_time);
    g_date_time_unref(now);

    return timespan;
}

gint deck_get_points(deck_t *deck)
{
    return 0;
}
