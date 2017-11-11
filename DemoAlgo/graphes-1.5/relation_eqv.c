/*
   On appelle aussi cette structure de donn�es : ensembles disjoints
 
   Il s'agit d'un ensemble d'arbres de sommets o� chaque fils pointe sur 
   son p�re. Deux sommets sont �quivalents si et seulement si ils 
   appartiennent au m�me arbre.

   Pour tout sommet x, la racine de l'arbre auquel x appartient est
   choisi comme repr�sentant canonique de la classe d'�quivalence de x.

   Deux heuristiques sont possibles :
   1/ �viter d'augmenter la hauteur des arbres au cours des fusions,
   2/ compresser les chemins.

   Seule la premi�re est implant�e (pour ne pas trop compliquer la lecture
   des affichages graphiques).
*/

#include "relation_eqv.h"

/*
   Chaque sommet n'est �quivalent qu'� lui-m�me
*/

void vider_relation_eqv (relation_eqv R)
{   sommet x;

    for (x = 0; x < n; x++)
    {	R [x].pere = PAS_UN_SOMMET;
	R [x].c = 0;
    }
}

/*
   Retourne le repr�sentant canonique du sommet
*/

static sommet representant (sommet s, relation_eqv R)
{   sommet x;

    x = s;
    while (R [x].pere != PAS_UN_SOMMET)
	x = R [x].pere;
    return x;
}

/*
   Fusionne les classes d'�quivalence des deux sommets

   La subtilit� algorithmique consiste � faire pointer le repr�sentant
   canonique de l'une des deux classes sur le repr�sentant canonique de
   l'autre.

   Soient x et y les deux repr�sentants canoniques. Qui doit pointer sur qui ?
   On cherche � ne pas augmenter la hauteur du nouvel arbre. C'est une
   heuristique destin�e � garantir une bonne complexit�.
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
   Retourne true si s et t appartiennent � la m�me classe et false sinon
*/

bool sont_equivalents_relation_eqv (sommet s, sommet t, relation_eqv R)
{
    return representant (s, R) == representant (t, R);
}

