#ifndef RELATION_EQV_H
#define RELATION_EQV_H 1

/*
   On appelle aussi cette structure de donn�es : ensembles disjoints

   Il s'agit d'un ensemble d'arbres de sommets o� chaque fils pointe sur 
   son p�re. Deux sommets sont �quivalents si et seulement si ils 
   appartiennent au m�me arbre.

   Pour tout sommet x, la racine de l'arbre auquel x appartient est
   choisi comme repr�sentant canonique de la classe d'�quivalence de x.
*/

#include <stdbool.h>
#include "def_graphe.h"
#include "un_graphe.h"

typedef struct {
    sommet pere;	/* pointeur sur le p�re */
    int c;		/* pour tout sommet i, relation_eqv [i].c est la
			   hauteur de l'arbre de racine i. Usage heuristique.
			*/
} relation_eqv [n];

extern void vider_relation_eqv (relation_eqv);
extern void relier_relation_eqv (sommet, sommet, relation_eqv);
extern bool sont_equivalents_relation_eqv (sommet, sommet, relation_eqv);

#endif /* !RELATION_EQV */
