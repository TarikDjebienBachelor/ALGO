#ifndef TABLEAU_ARCS_H
#define TABLEAU_ARCS_H 1

#include "def_graphe.h"
#include "un_graphe.h"

struct arc {
    sommet x, y;
};

struct tableau_arcs {
    struct arc tab [n*n];
    int fin;			/* Premi�re entr�e libre */
};

extern void vider_tableau_arcs (struct tableau_arcs*);
extern void ajouter_tableau_arcs (sommet, sommet, struct tableau_arcs*);

#endif /* !TABLEAU_ARCS_H */
