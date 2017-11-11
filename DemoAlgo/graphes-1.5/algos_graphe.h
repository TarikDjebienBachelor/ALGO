#ifndef ALGOS_GRAPHE_H
#define ALGOS_GRAPHE_H 1

#include "file.h"
#include "pile.h"
#include "tableau_sommets.h"
#include "tas.h"
#include "tableau_arcs.h"
#include "relation_eqv.h"

enum algorithme { largeur, profondeur, topologique, niveaux, Dijkstra, 
						EdmondsKarp, Kruskal };

extern struct file une_file;
extern struct pile une_pile;
extern struct tableau_sommets un_tableau_sommets;
						/* tri topologique */
extern struct tas un_tas;			/* Dijkstra */

extern int flux [n][n];				/* Edmonds-Karp */
extern struct tableau_arcs un_tableau_arcs;	/* Edmonds-Karp + Kruskal */

extern relation_eqv une_relation_eqv;		/* Kruskal */

extern void algo_largeur (sommet s);
extern void algo_profondeur (sommet s);
extern void algo_topologique (sommet s);
extern void algo_niveaux (sommet s);
extern void algo_Dijkstra (sommet s);
extern void algo_EdmondsKarp (sommet s, sommet t); 
extern void algo_Kruskal ();

#endif /* !ALGOS_GRAPHE_H */
