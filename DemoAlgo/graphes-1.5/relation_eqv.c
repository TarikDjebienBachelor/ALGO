/*
   On appelle aussi cette structure de données : ensembles disjoints
 
   Il s'agit d'un ensemble d'arbres de sommets où chaque fils pointe sur 
   son père. Deux sommets sont équivalents si et seulement si ils 
   appartiennent au même arbre.

   Pour tout sommet x, la racine de l'arbre auquel x appartient est
   choisi comme représentant canonique de la classe d'équivalence de x.

   Deux heuristiques sont possibles :
   1/ éviter d'augmenter la hauteur des arbres au cours des fusions,
   2/ compresser les chemins.

   Seule la première est implantée (pour ne pas trop compliquer la lecture
   des affichages graphiques).
*/

#include "relation_eqv.h"

/*
   Chaque sommet n'est équivalent qu'à lui-même
*/

void vider_relation_eqv (relation_eqv R)
{   sommet x;

    for (x = 0; x < n; x++)
    {	R [x].pere = PAS_UN_SOMMET;
	R [x].c = 0;
    }
}

/*
   Retourne le représentant canonique du sommet
*/

static sommet representant (sommet s, relation_eqv R)
{   sommet x;

    x = s;
    while (R [x].pere != PAS_UN_SOMMET)
	x = R [x].pere;
    return x;
}

/*
   Fusionne les classes d'équivalence des deux sommets

   La subtilité algorithmique consiste à faire pointer le représentant
   canonique de l'une des deux classes sur le représentant canonique de
   l'autre.

   Soient x et y les deux représentants canoniques. Qui doit pointer sur qui ?
   On cherche à ne pas augmenter la hauteur du nouvel arbre. C'est une
   heuristique destinée à garantir une bonne complexité.
*/

void relier_relation_eqv (sommet s, sommet t, relation_eqv R)
{   sommet x, y;

    x = representant (s, R);
    y = representant (t, R);
    if (x != y)
    {	if (R [x].c > R [y].c)
	    R [y].pere = x;
	else if (R [x].c < R [y].c)
	    R [x].pere = y;
	else
	{   R [x].pere = y;
	    R [y].c += 1;
	}
    }
}

/*
   Retourne true si s et t appartiennent à la même classe et false sinon
*/

bool sont_equivalents_relation_eqv (sommet s, sommet t, relation_eqv R)
{
    return representant (s, R) == representant (t, R);
}

