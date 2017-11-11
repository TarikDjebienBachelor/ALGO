#ifndef RELATION_EQV_H
#define RELATION_EQV_H 1

/*
   On appelle aussi cette structure de données : ensembles disjoints

   Il s'agit d'un ensemble d'arbres de sommets où chaque fils pointe sur 
   son père. Deux sommets sont équivalents si et seulement si ils 
   appartiennent au même arbre.

   Pour tout sommet x, la racine de l'arbre auquel x appartient est
   choisi comme représentant canonique de la classe d'équivalence de x.
*/

#include <stdbool.h>
#include "def_graphe.h"
#include "un_graphe.h"

typedef struct {
    sommet pere;	/* pointeur sur le père */
    int c;		/* pour tout sommet i, relation_eqv [i].c est la
			   hauteur de l'arbre de racine i. Usage heuristique.
			*/
} relation_eqv [n];

extern void vider_relation_eqv (relation_eqv);
extern void relier_relation_eqv (sommet, sommet, relation_eqv);
extern bool sont_equivalents_relation_eqv (sommet, sommet, relation_eqv);

#endif /* !RELATION_EQV */
