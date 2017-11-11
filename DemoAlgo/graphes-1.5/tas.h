#ifndef TAS_H
#define TAS_H 1

#include <stdbool.h>
#include "def_graphe.h"
#include "un_graphe.h"

struct tas {
    sommet tab [n];
    int pos [n];	/* pos [i] = l'indice dans tab du sommet i 
						si i est présent dans tab */
    int fin;		/* L'indice du premier emplacement libre */
};

extern void vider_tas (struct tas* tas);
extern bool est_vide_tas (struct tas* tas);
extern sommet extraire_tas (struct tas* tas);
extern void pousser_tas (sommet s, struct tas* tas);	/* s déjà présent */
extern void inserer_tas (sommet s, struct tas* tas);	/* s nouveau sommet */

#endif /* !TAS_H */
