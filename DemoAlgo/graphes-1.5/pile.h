#ifndef PILE_H
#define PILE_H 1

#include <stdbool.h>
#include "def_graphe.h"
#include "un_graphe.h"

struct pile {
    sommet tab [n];
    int top;
};

extern void vider_pile (struct pile* pile);
extern bool est_vide_pile (struct pile* pile);
extern void empiler_pile (sommet s, struct pile* pile);
extern sommet sommet_pile (struct pile* pile);
extern sommet depiler_pile (struct pile* pile);

#endif /* !PILE_H */

