#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

#include "algos_graphe.h"
#include "gnuplot.h"

static void usage (char* progname)
{
    fprintf (stderr, "usage:\n");
    fprintf (stderr, 
	"\t%s largeur nb_fenetres sommet_de_depart\n", progname);
    fprintf (stderr, 
	"\t%s profondeur nb_fenetres sommet_de_depart\n", progname);
    fprintf (stderr,
	"\t%s topologique nb_fenetres sommet_de_depart\n", progname);
    fprintf (stderr, 
	"\t%s niveaux nb_fenetres sommet_de_depart\n", progname);
    fprintf (stderr, 
	"\t%s Dijkstra nb_fenetres sommet_de_depart\n", progname);
    fprintf (stderr, 
	"\t%s EdmondsKarp nb_fenetres sommet_source sommet_destination\n", 
	progname);
    fprintf (stderr, 
	"\t%s Kruskal nb_fenetres\n", progname);
    fprintf (stderr, 
	"nb_fenetres = 1 ou 2\n");
    fprintf (stderr, 
	"pour les sommets, utiliser les lettres : A, B, C, ...\n");
    exit (1);
}

#include "gnuplot_version.h"

int main (int argc, char** argv)
{   sommet s, t;
    int nb_fenetres;

#ifdef NO_GNUPLOT
    fprintf (stderr, "Le logiciel gnuplot est introuvable\n");
    exit (1);
#endif

    if (argc >= 3)
    {	nb_fenetres = argv [2][0] - '0';
	if (nb_fenetres < 1 || nb_fenetres > 2) usage (argv [0]);
    } if (argc >= 4)
    {	s = argv [3][0] - 'A';
	if (s < 0) usage (argv [0]);
	if (s >= n) s = n-1;
    }
    if (argc >= 5)
    {	t = argv [4][0] - 'A';
	if (t < 0) usage (argv [0]);
	if (t >= n) t = n-1;
    }

    if (argc == 4 && strcmp (argv [1], "largeur") == 0)
    {   start_gnuplot (largeur, nb_fenetres);
	algo_largeur (s);
    } else if (argc == 4 && strcmp (argv [1], "profondeur") == 0)
    {	start_gnuplot (profondeur, nb_fenetres);
	algo_profondeur (s);
    } else if (argc == 4 && strcmp (argv [1], "topologique") == 0)
    {	start_gnuplot (topologique, nb_fenetres);
	algo_topologique (s);
    } else if (argc == 4 && strcmp (argv [1], "niveaux") == 0)
    {	start_gnuplot (niveaux, nb_fenetres);
	algo_niveaux (s);
    } else if (argc == 4 && strcmp (argv [1], "Dijkstra") == 0)
    {	start_gnuplot (Dijkstra, nb_fenetres);
	algo_Dijkstra (s);
    } else if (argc == 5 && strcmp (argv [1], "EdmondsKarp") == 0)
    {	start_gnuplot (EdmondsKarp, nb_fenetres);
	algo_EdmondsKarp (s, t);
    } else if (argc == 3 && strcmp (argv [1], "Kruskal") == 0)
    {	start_gnuplot (Kruskal, nb_fenetres);
	algo_Kruskal ();
    } else
	usage (argv [0]);

    stop_gnuplot ();

    return (0);
}

