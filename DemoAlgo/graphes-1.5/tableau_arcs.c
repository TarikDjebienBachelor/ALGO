#include "tableau_arcs.h"

void vider_tableau_arcs (struct tableau_arcs* c)
{
    c->fin = 0;
}

/*
   Ajout en queue
*/

void ajouter_tableau_arcs (sommet x, sommet y, struct tableau_arcs* c)
{
    c->tab [c->fin].x = x;
    c->tab [c->fin].y = y;
    c->fin++;
}
