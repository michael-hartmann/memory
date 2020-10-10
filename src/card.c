#include <gtk/gtk.h>
#include <string.h>

#include "card.h"

card_t *card_new(void *deck, const gchar *filename_image, const gchar *filename_cover)
{
    card_t *card = g_malloc(sizeof(card_t));

    card->deck = deck;
    card->filename_image = NULL;
    card->filename_cover = NULL;

    card_set_image(card, filename_image);
    card_set_cover(card, filename_cover);

    card->button = gtk_button_new();
    card_set_hidden(card);

    return card;
}

void card_set_shown(card_t *card)
{
    gtk_button_set_image(GTK_BUTTON(card->button), gtk_image_new_from_file(card->filename_image));
    card->state = CARD_SHOWN;
}

void card_set_hidden(card_t *card)
{
    gtk_button_set_image(GTK_BUTTON(card->button), gtk_image_new_from_file(card->filename_cover));
    card->state = CARD_HIDDEN;
}

void card_set_solved(card_t *card)
{
    gtk_button_set_image(GTK_BUTTON(card->button), gtk_image_new_from_file(card->filename_image));
    gtk_widget_set_sensitive(card->button, FALSE);
    card->state = CARD_SOLVED;
}

void card_free(card_t *card)
{
    if(card != NULL)
    {
        if(card->filename_cover != NULL)
            g_free(card->filename_cover);
        if(card->filename_image != NULL)
            g_free(card->filename_image);

        g_free(card);
    }
}

gchar *card_get_cover(card_t *card)
{
    return card->filename_cover;
}

GtkWidget *card_get_button(card_t *card)
{
    return card->button;
}

gchar *card_get_image(card_t *card)
{
    return card->filename_image;
}

void card_set_image(card_t *card, const gchar *filename)
{
    if(card->filename_image != NULL)
        g_free(card->filename_image);

    card->filename_image = g_strdup(filename);
}


void card_set_cover(card_t *card, const gchar *filename)
{
    if(card->filename_cover != NULL)
        g_free(card->filename_cover);

    card->filename_cover = g_strdup(filename);
}

gint card_compare(card_t *card1, card_t *card2)
{
    return g_strcmp0(card1->filename_image, card2->filename_image);
}

gint card_get_state(card_t *card)
{
    return card->state;
}
