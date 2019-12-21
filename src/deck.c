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

deck_t *deck_new(const gchar *directory, const gchar *cover, gint pairs)
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
    deck->elements = 2*pairs;
    deck->cards    = g_malloc(2*pairs*sizeof(card_t *));

    for(gint i = 0; i < pairs; i++)
    {
        deck->cards[2*i]   = card_new(list[i], cover);
        deck->cards[2*i+1] = card_new(list[i], cover);
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
        for(gint i = 0; i < deck->elements; i++)
            card_free(deck->cards[i]);

        g_free(deck->cards);
        g_free(deck);
    }
}
