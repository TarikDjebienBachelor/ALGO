#include <stdio.h>
#include <stdlib.h>
#include "algos_graphe.h"
#include "gnuplot.h"

struct file une_file;
struct pile une_pile;
struct tableau_sommets un_tableau_sommets;
					/* tri topologique */

struct tas un_tas;			/* Dijkstra */

int flux [n][n];			/* Edmonds-Karp */

struct tableau_arcs un_tableau_arcs;	/* Edmonds-Karp + Kruskal */

relation_eqv une_relation_eqv;		/* Kruskal */

/***********************************************************************
   PARCOURS EN LARGEUR D'ABORD
***********************************************************************/

void algo_largeur (sommet s)
{   sommet x, y;

    for (x = 0; x < n; x++)
    {	S [x].c = bleu;
	S [x].pred = PAS_UN_SOMMET;
    }
    vider_file (&une_file);
    S [s].c = vert;
    enfiler_file (s, &une_file);
    while (update_gnuplot (largeur), ! est_vide_file (&une_file))
    {   x = defiler_file (&une_file);
	for (y = 0; y < n; y++)
	{   if (B [x][y] != 0 && S [y].c == bleu)
	    {	S [y].c = vert;
		S [y].pred = x;
		enfiler_file (y, &une_file);
	    }
	}
	S [x].c = rouge;
    }
}

/***********************************************************************
   PARCOURS EN PROFONDEUR D'ABORD
***********************************************************************/

void algo_profondeur (sommet s)
{   sommet x, y;
    bool found;

    for (x = 0; x < n; x++)
    {	S [x].c = bleu;
	S [x].pred = PAS_UN_SOMMET;
	S [x].i = -1;
    }
    vider_pile (&une_pile);
    S [s].c = vert;
    empiler_pile (s, &une_pile);
    while (update_gnuplot (profondeur), ! est_vide_pile (&une_pile))
    {	x = sommet_pile (&une_pile);
/*
   Recherche du premier successeur bleu d'indice supérieur à S [x].i
*/
	S [x].i += 1;
        found = false;
	while (S [x].i < n && ! found)
	{   y = S [x].i;
	    if (B [x][y] != 0 && S [y].c == bleu)
		found = true;
	    else
		S [x].i += 1;
	}
/*
   Si x a un successeur bleu y ...
*/
	if (found)
	{   S [y].c = vert;
	    S [y].pred = x;
	    empiler_pile (y, &une_pile);
	} else
	{   S [x].c = rouge;
	    depiler_pile (&une_pile);
	}
    }
}

/**********************************************************************
 TRI TOPOLOGIQUE

  Le tableau de sommets reçoit les sommets triés par ordre topologique
  Le tableau d'arcs reçoit les éventuels arcs arrière détectés qui
	prouveraient que le graphe comporte des circuits.
 **********************************************************************/

void algo_topologique (sommet s)
{   sommet x, y;
    bool found;

    vider_tableau_arcs (&un_tableau_arcs);
    for (x = 0; x < n; x++)
    {	S [x].c = bleu;
	S [x].pred = PAS_UN_SOMMET;
	S [x].i = -1;
    }
    vider_pile (&une_pile);
    vider_tableau_sommets (&un_tableau_sommets);
    S [s].c = vert;
    empiler_pile (s, &une_pile);
    while (update_gnuplot (topologique), ! est_vide_pile (&une_pile))
    {	x = sommet_pile (&une_pile);
/*
   Recherche du premier successeur bleu d'indice supérieur à S [x].i
*/
	S [x].i += 1;
        found = false;
	while (S [x].i < n && ! found)
	{   y = S [x].i;
	    if (B [x][y] != 0 && S [y].c == bleu)
		found = true;
	    else 
	    {	if (B [x][y] != 0 && S [y].c == vert)
		    ajouter_tableau_arcs (x, y, &un_tableau_arcs);
		S [x].i += 1;
	    }
	}
/*
   Si x a un successeur bleu y ...
*/
	if (found)
	{   S [y].c = vert;
	    S [y].pred = x;
	    empiler_pile (y, &une_pile);
	} else
	{   S [x].c = rouge;
	    depiler_pile (&une_pile);
	    ajouter_tableau_sommets (x, &un_tableau_sommets);
	}
    }
}

/***********************************************************************
   CALCUL DES NIVEAUX

   Calcul des niveaux à partir de s. Implantation au moyen d'une file.
   Suppose le graphe sans circuit.
***********************************************************************/

void algo_niveaux (sommet s)
{   sommet x, y;

    for (x = 0; x < n; x++)
    {	S [x].c = bleu;
	S [x].niv = 0;
	S [x].cpt = 0;
	S [x].pred = PAS_UN_SOMMET;
	for (y = 0; y < n; y++)
	    if (B [y][x] != 0)
		S [x].cpt++;
    }
    S [s].c = vert;
    vider_file (&une_file);
    enfiler_file (s, &une_file);
    while (update_gnuplot (niveaux), ! est_vide_file (&une_file))
    {	x = defiler_file (&une_file);
	S [x].c = rouge;
	for (y = 0; y < n; y++)
	{   if (B [x][y] != 0)
	    {	S [y].cpt--;
	    	if (S [y].cpt == 0)
	    	{   S [y].c = vert;
		    enfiler_file (y, &une_file);
	    	}
/*
   Le calcul du niveau est un calcul de maximum : 
	niveau (x) = 1 + maximum niveau (y) pour y prédecesseur de x.
   Ce calcul de maximum se fait incrémentalement.
*/ 
		if (1 + S [x].niv > S [y].niv)
		{   S [y].niv = 1 + S [x].niv;
		    S [y].pred = x;
		}
	    }
	}
    }
}

/***********************************************************************
   PLUS COURT CHEMIN

   Plus court chemin dans un graphe éventuellement avec circuits mais
valué positivement. L'algorithme d'Edsger Wybe Dijkstra, implanté au
moyen d'un tas.
***********************************************************************/

void algo_Dijkstra (sommet s)
{   sommet x, y;

    for (x = 0; x < n; x++)
    {	S [x].c = bleu;
	S [x].pred = PAS_UN_SOMMET;
    }
    vider_tas (&un_tas);
    S [s].pi = 0;
    S [s].c = vert;
    inserer_tas (s, &un_tas);
/*
   L'ensemble des sommets verts est géré au moyen d'une file avec priorité,
   implantée sous la forme d'un tas.
*/
    while (update_gnuplot (Dijkstra), ! est_vide_tas (&un_tas))
    {	x = extraire_tas (&un_tas);
	S [x].c = rouge;
	for (y = 0; y < n; y++)
	{   if (B [x][y] != 0)
	    {	
/*
   Attention à gérer différemment les sommets bleus, qui ne sont pas 
   dans le tas, des sommets verts qui sont, eux, déjà présents dans le tas.

   Le calcul de pi est un calcul de minimum. Ce minimum est calculé 
   incrémentalement.
*/
		if (S [y].c == bleu)
		{   S [y].c = vert;
		    S [y].pred = x;
		    S [y].pi = S [x].pi + B [x][y];
		    inserer_tas (y, &un_tas);
		} else if (S [y].c == vert && S [y].pi > S [x].pi + B [x][y])
		{   S [y].pred = x;
		    S [y].pi = S [x].pi + B [x][y];
		    pousser_tas (y, &un_tas);
		}
	    }
	}
    }
}

/**********************************************************************
   FLOT MAXIMAL

   Algorithme d'Edmonds Karp.
   Variante de Ford Fulkerson dans laquelle la chaîne améliorante est
calculée en largeur d'abord.

   Le graphe est un réseau de transport.
***********************************************************************/

static bool chaine_ameliorante (sommet, sommet);
static int  delta (sommet, sommet);
static void augmenter_le_flot (sommet, sommet, int);

void algo_EdmondsKarp (sommet s, sommet t)
{   sommet x, y;
    bool b;
    int d;

    for (x = 0; x < n; x++)
	for (y = 0; y < n; y++)
	    flux [x][y] = 0;
    while (b = chaine_ameliorante (s, t), update_gnuplot (EdmondsKarp), b)
    {	d = delta (s, t);
	augmenter_le_flot (s, t, d);
    }
}

/*
   Retourne true s'il existe un arc direct de x vers y non saturé
*/

static bool arc_direct_non_sature (sommet x, sommet y)
{
    return B [x][y] != 0 && flux [x][y] < B [x][y];
}

/*
   Retourne true s'il existe un arc indirect de x vers y non saturé
*/

static bool arc_indirect_non_sature (sommet x, sommet y)
{
    return B [y][x] != 0 && flux [y][x] > 0;
}

/*
   Retourne true s'il existe une chaîne améliorante entre s et t.
   Si c'est le cas, la chaîne s'obtient en suivant les pointeurs pred
à partir de t. Dans la variante d'Edmonds Karp du schéma d'algorithme
de Ford Fulkerson, la chaîne améliorante est calculée grâce à un parcours
en largeur d'abord.
*/

static void enregistre_chaine_ameliorante 
				(sommet, sommet, struct tableau_arcs*);

static bool chaine_ameliorante (sommet s, sommet t)
{   sommet x, y;

    for (x = 0; x < n; x++)
	S [x].c = bleu;
    vider_file (&une_file);
    S [s].pred = PAS_UN_SOMMET;
    S [s].c = rouge;
    enfiler_file (s, &une_file);
    while (! est_vide_file (&une_file) && S [t].c == bleu)
    {	x = defiler_file (&une_file);
	for (y = 0; y < n; y++)
	{   if (S [y].c == bleu && (arc_direct_non_sature (x, y) 
					|| arc_indirect_non_sature (x, y)))
	    {	S [y].pred = x;
		S [y].c = rouge;
		enfiler_file (y, &une_file);
	    }
	}
    }
/*
   Pour affichage uniquement (ne contribue pas à l'algorithme)
*/
    if (S [t].c == rouge)
	enregistre_chaine_ameliorante (s, t, &un_tableau_arcs);
    else
	vider_tableau_arcs (&un_tableau_arcs);
    return S [t].c == rouge;
}

/*
   Pour affichage uniquement (ne contribue pas à l'algorithme)
*/

static void enregistre_chaine_ameliorante 
			(sommet s, sommet t, struct tableau_arcs* c)
{   sommet x, y;

    vider_tableau_arcs (c);
    y = t;
    x = S [y].pred;
    if (arc_direct_non_sature (x, y))
	ajouter_tableau_arcs (x, y, c);
    else
	ajouter_tableau_arcs (y, x, c);
    y = x;
    while (y != s)
    {   x = S [y].pred;
        if (arc_direct_non_sature (x, y))
	    ajouter_tableau_arcs (x, y, c);
        else
            ajouter_tableau_arcs (y, x, c);
        y = x;
    }
}

/*
   Retourne une augmentation possible du flux le long de la chaîne améliorante.
   C'est en général l'augmentation maximale possible le long de cette chaîne.
*/

int delta (sommet s, sommet t)
{   sommet x, y;
    int d, min;

    y = t;
    x = S [y].pred;
    if (arc_direct_non_sature (x, y))
	d = B [x][y] - flux [x][y];
    else
	d = flux [y][x];
    min = d;
    y = x;
    while (y != s)
    {	x = S [y].pred;
	if (arc_direct_non_sature (x, y))
	    d = B [x][y] - flux [x][y];
	else
	    d = flux [y][x];
	if (d < min) min = d;
	y = x;
    }
    return min;
}

/*
   Augmente de delta le flux le long de la chaîne améliorante.
*/

static void augmenter_le_flot (sommet s, sommet t, int delta)
{   sommet x, y;
    y = t;
    x = S [y].pred;
    if (arc_direct_non_sature (x, y))
	flux [x][y] += delta;
    else
	flux [y][x] -= delta;
    y = x;
    while (y != s)
    {   x = S [y].pred;
        if (arc_direct_non_sature (x, y))
	    flux [x][y] += delta;
	else
	    flux [y][x] -= delta;
	y = x;
    }
}

/**********************************************************************
 ARBRE COUVRANT MINIMAL

 Algorithme de Kruskal
 **********************************************************************/

/*
   Retourne true si le graphe est non orienté et false sinon.
*/

static bool est_non_oriente ()
{   sommet x, y;

    for (x = 0; x < n; x++)
	for (y = x+1; y < n; y++)
	    if (B [x][y] != B [y][x])
		return false;
    return true;
}

/*
   Pour quicksort
*/

static int compare_arcs (const void* s, const void* t)
{   struct arc* a = (struct arc*)s;
    struct arc* b = (struct arc*)t;
    int valeur_a, valeur_b;

    valeur_a = B [a->x][a->y];
    valeur_b = B [b->x][b->y];

    return valeur_a < valeur_b ? -1 : valeur_a == valeur_b ? 0 : 1;
}

void algo_Kruskal ()
{   sommet x, y;
    struct tableau_arcs arcs;
    int i;

    if (! est_non_oriente ())
    {	fprintf (stderr, 
	    "erreur: algorithme de Kruskal appliqué à un graphe orienté\n");
	exit (1);
    }

    vider_tableau_arcs (&arcs);
    for (x = 0; x < n; x++)
	for (y = x+1; y < n; y++)
	    if (B [x][y] != 0)
		ajouter_tableau_arcs (x, y, &arcs);
    qsort (arcs.tab, arcs.fin, sizeof (struct arc), &compare_arcs);

    vider_relation_eqv (une_relation_eqv);

    vider_tableau_arcs (&un_tableau_arcs);
    i = -1;
/*
   Double boucle un peu alambiquée pour que les affichages soient agréables.
*/
    while (update_gnuplot (Kruskal), un_tableau_arcs.fin < n - 1)
    {	do
	{   i += 1;
	    x = arcs.tab [i].x;
	    y = arcs.tab [i].y;
	} while (sont_equivalents_relation_eqv (x, y, une_relation_eqv));
	relier_relation_eqv (x, y, une_relation_eqv);
	ajouter_tableau_arcs (x, y, &un_tableau_arcs);
    }
}
