#ifndef MEMORY_H
#define MEMORY_H

#include <gtk/gtk.h>
#include "deck.h"

typedef struct {
    deck_t *deck;
    GKeyFile *keyfile;
    GtkWidget *window, *vbox, *scrolled_window, *menu_new, *status;
} memory_t;

memory_t *memory_create(void);
void memory_destroy(memory_t *memory);

#endif
