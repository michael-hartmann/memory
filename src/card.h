#ifndef __CARD_H
#define __CARD_H

    #define CARD_SHOWN  1
    #define CARD_HIDDEN 0
    #define CARD_SOLVED 3

    typedef struct {
        void *deck;
        gchar *filename_image, *filename_cover;
        gint state;
        GtkWidget *button;
    } card_t;

    card_t *card_new(void* game, const gchar *filename_image, const gchar *filename_cover);
    void card_free(card_t *card);
    GtkWidget *card_get_button(card_t *card);

    void card_set_shown(card_t *card);
    void card_set_hidden(card_t *card);
    void card_set_solved(card_t *card);

    void card_set_cover(card_t *card, const gchar *filename);

    int card_compare(card_t *card1, card_t *card2);

    gchar *card_get_image(card_t *card);
    void card_set_image(card_t *card, const gchar *filename);

    gchar *card_get_cover(card_t *card);
    void card_set_cover(card_t *card, const gchar *filename);

    void card_hide(card_t *card);
    void card_show(card_t *card);

    gint card_compare(card_t *card1, card_t *card2);
    gint card_get_state(card_t *card);

#endif
