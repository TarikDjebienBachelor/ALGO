#ifndef GNUPLOT_H
#define GNUPLOT_H 1

#include "algos_graphe.h"

/*
   Constantes liées entre elles et réglées empiriquement.
*/

#include "gnuplot_version.h"
#include "gnuplot_geometry.h"

/* 
   Le code pour le type "boîte carrée" a changé entre les versions 3 et 4
*/

#ifdef GNUPLOT_V3
#define POINTTYPE 3
#else
#define POINTTYPE 4
#endif

#define OFFSET 2
#define SHIFT  1
#define LINEWIDTH 2
#define LINETYPE1 1 		/* rouge */
#define LINETYPE2 4		/* rose */
#define POINTWIDTH 7
#define WINDOWWIDTH 20
#define WINDOWHEIGHT 20

extern void start_gnuplot (enum algorithme, int nb_fenetres);
extern void update_gnuplot (enum algorithme);
extern void stop_gnuplot ();

#endif /* !GNUPLOT_H */

