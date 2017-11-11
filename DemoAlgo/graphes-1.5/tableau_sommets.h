#ifndef TABLEAU_SOMMETS_H
#define TABLEAU_SOMMETS_H 1

#include "def_graphe.h"
#include "un_graphe.h"

/*
   Le tableau se remplit à partir de la fin
*/

struct tableau_sommets {
    sommet tab [n];
    int deb;				/* Première entrée libre */
};

extern void vider_tableau_sommets (struct tableau_sommets*);
extern void ajouter_tableau_sommets (sommet, struct tableau_sommets*);


#endif /* !TABLEAU_SOMMETS_H */
