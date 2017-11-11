#include <stdbool.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>

#include "gnuplot.h"

/*
   Les descripteurs de fichier des canaux de communication avec gnuplot
*/

#define PAS_OUVERT -1

static int gnuplot1;
static int gnuplot2;

/*
   Variante de printf. Imprime sur gnuplot.
*/

static void gnuplot_printf (int gnuplot, char* format, ...)
{   static char buffer [1024];
    va_list params;

    if (gnuplot != PAS_OUVERT)
    {	va_start (params, format);
	vsprintf (buffer, format, params);
	va_end (params);
	write (gnuplot, buffer, strlen (buffer));
    }
}

/*
   Variante de printf. Imprime le message sur la sortie d'erreur puis
   arrête le programme avec code de retour 1.
*/

static void error (char* format, ...)
{   static char buffer [1024];
    va_list params;

    va_start (params, format);
    vsprintf (buffer, format, params);
    va_end (params);
    fprintf (stderr, "%s\n", buffer);
    exit (1);
}

/*
   Enregistre dans le fichier fname les coordonnées des sommets de couleur c
*/

static void gen_sommets (char* fname, enum couleur c)
{   FILE* f;
    sommet i;

    f = fopen (fname, "w");
    if (f == NULL) error ("ouverture de %s impossible", fname);

    fprintf (f, "%d %d\n", 2*WINDOWWIDTH, 2*WINDOWHEIGHT);
    for (i = 0; i < n; i++)
	if (S [i].c == c)
	   fprintf (f, "%d %d\n", S [i].x, S [i].y);
    fclose (f);
}

/*
   Enregistre dans le fichier fname les coordonnées des arcs du graphe.
   Pour certains algorithmes, envoie à gnuplot des "labels" destinés à
afficher les valeurs des arcs.
*/

static void gen_fleches (int gnuplot, char* fname, enum algorithme algo)
{   FILE* f;
    sommet s, d;
    int dx, dy;

    f = fopen (fname, "w");
    if (f == NULL) error ("ouverture de %s impossible", fname);

    fprintf (f, "%d %d 1 1\n", 2*WINDOWWIDTH, 2*WINDOWHEIGHT);
    for (s = 0; s < n; s++)
    {	for (d = 0; d < n; d++)
	{   if (B [s][d] != 0)
	    {	dx = S [d].x - S [s].x;
		dy = S [d].y - S [s].y;
		fprintf (f, "%lf %lf %lf %lf\n", 
			(double)(S [s].x + 0.2 * dx),
			(double)(S [s].y + 0.2 * dy),
			(double)(0.6 * dx),
			(double)(0.6 * dy));
/*
   Annotation des flèches
*/
		switch (algo)
		{   case Dijkstra:
		    case Kruskal:
			gnuplot_printf (gnuplot, 
				"set label \"v=%d\" at %d,%d\n",
				 B [s][d], (S [d].x + S [s].x) / 2, 
					   (S [d].y + S [s].y) / 2);
			break;
		    case EdmondsKarp:
			gnuplot_printf (gnuplot,
				"set label \"%d/%d\" at %d,%d\n",
				 flux [s][d], B [s][d],
				 (S [d].x + S [s].x) / 2,
				 (S [d].y + S [s].y) / 2);
			break;
		    default:
			break;
		}
	    }
	}
    }
    fclose (f);
}

static void gen_tableau_arcs (char* fname)
{   FILE* f;
    sommet s, d;
    int i, dx, dy;

    f = fopen (fname, "w");
    if (f == NULL) error ("ouverture de %s impossible", fname);

    fprintf (f, "%d %d 1 1\n", 2*WINDOWWIDTH, 2*WINDOWHEIGHT);
    for (i = 0; i < un_tableau_arcs.fin; i++)
    {   s = un_tableau_arcs.tab [i].x;
	d = un_tableau_arcs.tab [i].y;
	dx = S [d].x - S [s].x;
        dy = S [d].y - S [s].y;
        fprintf (f, "%lf %lf %lf %lf\n",
                        (double)(S [s].x + 0.2 * dx),
                        (double)(S [s].y + 0.2 * dy),
                        (double)(0.6 * dx),
                        (double)(0.6 * dy));
    }
    fclose (f);
}

static void gen_pred (char* fname)
{   FILE* f;
    sommet s, d;
    int dx, dy;

    f = fopen (fname, "w");
    if (f == NULL) error ("ouverture de %s impossible", fname);

    fprintf (f, "%d %d 1 1\n", 2*WINDOWWIDTH, 2*WINDOWHEIGHT);
    for (s = 0; s < n; s++)
    {   d = S [s].pred;
	if (d != PAS_UN_SOMMET)
        {   dx = S [d].x - S [s].x;
            dy = S [d].y - S [s].y;
            fprintf (f, "%lf %lf %lf %lf\n",
                        (double)(S [s].x + 0.2 * dx),
                        (double)(S [s].y + 0.2 * dy),
                        (double)(0.6 * dx),
                        (double)(0.6 * dy));
	}
    }
    fclose (f);
}

static void gen_relation_equivalence (char* fname)
{   FILE* f;
    sommet s, d;
    int dx, dy;

    f = fopen (fname, "w");
    if (f == NULL) error ("ouverture de %s impossible", fname);

    fprintf (f, "%d %d 1 1\n", 2*WINDOWWIDTH, 2*WINDOWHEIGHT);
    for (s = 0; s < n; s++)
    {	if (une_relation_eqv [s].pere != PAS_UN_SOMMET)
	{   d = une_relation_eqv [s].pere;
            dx = S [d].x - S [s].x;
            dy = S [d].y - S [s].y;
            fprintf (f, "%lf %lf %lf %lf\n",
                        (double)(S [s].x + 0.2 * dx),
                        (double)(S [s].y + 0.2 * dy),
                        (double)(0.6 * dx),
                        (double)(0.6 * dy));
	}
     }
     fclose (f);
}
	    
/*
   Impression d'une file.
   Enregistre dans le fichier fname les coordonnées des boîtes de une_file.
   Envoie à gnuplot sous la forme de "labels" les noms des sommets
présents dans la file.
*/

static void gen_file (int gnuplot, char* fname)
{   FILE* f;
    int i;

    f = fopen (fname, "w");
    if (f == NULL) error ("ouverture de %s impossible", fname);

    gnuplot_printf (gnuplot, "set label \"file = \" at 1,%d\n", SHIFT);

    fprintf (f, "%d %d\n", 2*WINDOWWIDTH, 2*WINDOWHEIGHT);
    for (i = 0; (une_file.debut + i) % n != une_file.fin; i++)
    {	fprintf (f, "%d %d\n", OFFSET + (i + 1)*SHIFT, SHIFT);
	gnuplot_printf (gnuplot, "set label \"%c\" center at %d,%d\n",
			 'A' + une_file.tab [(une_file.debut + i) % n], 
			 OFFSET + (i + 1)*SHIFT, SHIFT);
    }
    fclose (f);
}

/*
   Impression d'une pile.
   Enregistre dans le fichier fname les coordonnées des boîtes de une_pile.
   Envoie à gnuplot sous la forme de "labels" les noms des sommets
présents dans la pile.
*/

static void gen_pile (int gnuplot, char* fname)
{   FILE* f;
    int i;

    f = fopen (fname, "w");
    if (f == NULL) error ("ouverture de %s impossible", fname);

    gnuplot_printf (gnuplot, "set label \"pile = \" at 1,%d\n", SHIFT);

    fprintf (f, "%d %d\n", 2*WINDOWWIDTH, 2*WINDOWHEIGHT);
    for (i = 0; i < une_pile.top; i++)
    {   fprintf (f, "%d %d\n", OFFSET + (i + 1)*SHIFT, SHIFT);
	gnuplot_printf (gnuplot, "set label \"%c\" center at %d,%d\n",
			'A' + une_pile.tab [i], 
			OFFSET + (i + 1)*SHIFT, SHIFT);
    }
    fclose (f);
}

static void gen_tableau_sommets  (int gnuplot, char* fname)
{   FILE* f;
    int i;

    f = fopen (fname, "w");
    if (f == NULL) error ("ouverture de %s impossible", fname);

    gnuplot_printf (gnuplot, "set label \"tab = \" at 1,%d\n", SHIFT);

    fprintf (f, "%d %d\n", 2*WINDOWWIDTH, 2*WINDOWHEIGHT);
    for (i = un_tableau_sommets.deb + 1; i < n; i++)
    {   fprintf (f, "%d %d\n", OFFSET + (i + 1)*SHIFT, SHIFT);
        gnuplot_printf (gnuplot, "set label \"%c\" center at %d,%d\n",
                        'A' + un_tableau_sommets.tab [i],
                        OFFSET + (i + 1)*SHIFT, SHIFT);
    }
    fclose (f);
}

/*
   Impression d'un tas.
   Enregistre dans le fichier fname les coordonnées des boîtes de un_tas.
   Envoie à gnuplot sous la forme de "labels" les noms des sommets
présents dans le tas.
*/

static void gen_tas (int gnuplot, char* fname)
{   FILE* f;
    int i;

    f = fopen (fname, "w");
    if (f == NULL) error ("ouverture de %s impossible", fname);

    gnuplot_printf (gnuplot, "set label \"tas = \" at 1,%d\n", SHIFT);

    fprintf (f, "%d %d\n", 2*WINDOWWIDTH, 2*WINDOWHEIGHT);
    for (i = 0; i < un_tas.fin; i++)
    {   fprintf (f, "%d %d\n", OFFSET + (i + 1)*SHIFT, SHIFT);
	gnuplot_printf (gnuplot, "set label \"%c\" center at %d,%d\n",
			'A' + un_tas.tab [i], 
			OFFSET + (i + 1)*SHIFT, SHIFT);
    }
    fclose (f);
}

/*
   Met à jour la fenêtre gnuplot puis attend qu'on tape sur entrée.
*/

void update_gnuplot (enum algorithme algo)
{   sommet i;
    FILE* f;

    f = fopen ("vide", "w");
    if (f == NULL) error ("ouverture de %s impossible", "vide");
    fclose (f);

    gnuplot_printf (gnuplot1, "set nolabel\n");
    gnuplot_printf (gnuplot2, "set nolabel\n");
    gnuplot_printf (gnuplot1, "set pointsize %d\n", POINTWIDTH);

    gen_sommets ("verts", vert);
    gen_sommets ("rouges", rouge);
    gen_sommets ("bleus", bleu);
    gen_fleches (gnuplot1, "fleches", algo);

    for (i = 0; i < n; i++)
    {	
/*
   Sommets fenêtre principale
*/
	gnuplot_printf (gnuplot1, "set label \"%c\" center at %d,%d\n", 
						'A' + i, S [i].x, S [i].y);
/*
   Sommets fenêtre secondaire
*/
	switch (algo)
	{   case largeur:
	    case profondeur:
	    case Dijkstra:
	    case niveaux:
		gnuplot_printf (gnuplot2, "set label \"%c\" center at %d,%d\n",
						'A' + i, S [i].x, S [i].y);
		break;
	    case Kruskal:
		gnuplot_printf (gnuplot2, "set label \"%c\" center at %d,%d\n",
						'A' + i, S [i].x, S [i].y);
		gnuplot_printf (gnuplot2, "set label \"h=%d\" at %d,%d\n",
						une_relation_eqv [i].c,
						S [i].x + 1, S [i].y - 1);
		break;
	    case topologique:
	    case EdmondsKarp:
		break;
	}
/*
   Annotation des sommets fenêtre principale
*/
	switch (algo)
	{   case niveaux:
		gnuplot_printf (gnuplot1, "set label \"n=%d\" at %d,%d\n",
					S [i].niv, S [i].x + 1, S [i].y - 1);
		gnuplot_printf (gnuplot1, "set label \"c=%d\" at %d,%d\n",
                                        S [i].cpt, S [i].x + 1, S [i].y - 2);
		break;
	    case Dijkstra:
		if (S [i].c != bleu)
		    gnuplot_printf (gnuplot1, "set label \"pi=%d\" at %d,%d\n",
					S [i].pi, S [i].x + 1, S [i].y - 1);
		break;
	    default:
		break;
	}
    }
/*
   Structures spéciales (file, pile, tas ou sous-ensemble des arcs)
   Et affichage.
*/
    switch (algo)
    {	case largeur:
	case niveaux:
	    gen_file (gnuplot1, "structure");
	    gen_pred ("arbre");
	    gnuplot_printf (gnuplot2, "plot [0:%d] [0:%d] \
		\"%s\" notitle with vector linewidth 2\n",
			WINDOWWIDTH, WINDOWHEIGHT, "arbre");
	    gnuplot_printf (gnuplot1, "plot [0:%d] [0:%d] \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with vector linewidth %d, \
		\"%s\" notitle with points pointtype %d\n", 
			WINDOWWIDTH, WINDOWHEIGHT, 
			"rouges", POINTTYPE,
			"verts", POINTTYPE,
			"bleus", POINTTYPE, 
			"fleches", LINEWIDTH,
			"structure", POINTTYPE);
	    break;
	case profondeur:
	    gen_pile (gnuplot1, "structure");
            gen_pred ("arbre");
	    gnuplot_printf (gnuplot2, "plot [0:%d] [0:%d] \
		\"%s\" notitle with vector linewidth 2\n",
			WINDOWWIDTH, WINDOWHEIGHT, "arbre");
	    gnuplot_printf (gnuplot1, "plot [0:%d] [0:%d] \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with vector linewidth %d, \
		\"%s\" notitle with points pointtype %d\n", 
			WINDOWWIDTH, WINDOWHEIGHT, 
			"rouges", POINTTYPE, 
			"verts", POINTTYPE,
			"bleus", POINTTYPE, 
			"fleches", LINEWIDTH,
			"structure", POINTTYPE);
	    break;
	case topologique:
	    gen_tableau_sommets (gnuplot1, "structure");
	    gen_tableau_arcs ("tabarcs");
	    gnuplot_printf (gnuplot1, "plot [0:%d] [0:%d] \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with vector linetype %d linewidth %d, \
		\"%s\" notitle with vector linetype %d linewidth %d, \
		\"%s\" notitle with points pointtype %d\n", 
			WINDOWWIDTH, WINDOWHEIGHT, 
			"rouges", POINTTYPE, 
			"verts", POINTTYPE,
			"bleus", POINTTYPE, 
			"fleches", LINETYPE1, LINEWIDTH,
			"tabarcs", LINETYPE2, LINEWIDTH,
			"structure", POINTTYPE);
	    break;
	case Dijkstra:
	    gen_tas (gnuplot1, "structure");
            gen_pred ("arbre");
	    gnuplot_printf (gnuplot2, "plot [0:%d] [0:%d] \
		\"%s\" notitle with vector linewidth 2\n",
			WINDOWWIDTH, WINDOWHEIGHT, "arbre");
	    gnuplot_printf (gnuplot1, "plot [0:%d] [0:%d] \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with vector linewidth %d, \
		\"%s\" notitle with points pointtype %d\n", 
			WINDOWWIDTH, WINDOWHEIGHT, 
			"rouges", POINTTYPE,
			"verts", POINTTYPE,
			"bleus", POINTTYPE, 
			"fleches", LINEWIDTH,
			"structure", POINTTYPE);
	    break;
	case EdmondsKarp:
	    gen_tableau_arcs ("tabarcs");
	    gnuplot_printf (gnuplot1, "plot [0:%d] [0:%d] \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with vector linetype %d linewidth %d, \
		\"%s\" notitle with vector linetype %d linewidth %d\n", 
			WINDOWWIDTH, WINDOWHEIGHT, 
			"rouges", POINTTYPE,
			"verts", POINTTYPE,
			"bleus", POINTTYPE, 
			"fleches", LINETYPE1, LINEWIDTH,
			"tabarcs", LINETYPE2, LINEWIDTH);
	    break;
	case Kruskal:
	    gen_relation_equivalence ("structure");
	    gnuplot_printf (gnuplot2, "plot [0:%d] [0:%d] \
		\"%s\" notitle with vector linetype %d linewidth %d\n",
			WINDOWWIDTH, WINDOWHEIGHT,
			"structure", LINETYPE1, LINEWIDTH);
	    gen_tableau_arcs ("tabarcs");
	    gnuplot_printf (gnuplot1, "plot [0:%d] [0:%d] \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with points pointtype %d, \
		\"%s\" notitle with vector linetype %d linewidth %d, \
		\"%s\" notitle with vector linetype %d linewidth %d\n", 
			WINDOWWIDTH, WINDOWHEIGHT, 
			"rouges", POINTTYPE,
			"verts", POINTTYPE,
			"bleus", POINTTYPE, 
			"fleches", LINETYPE1, LINEWIDTH,
			"tabarcs", LINETYPE2, LINEWIDTH);
	    break;
    }

    getchar ();

    unlink ("vide");
    unlink ("rouges");
    unlink ("verts");
    unlink ("bleus");
    unlink ("fleches");
    unlink ("tabarcs");
    unlink ("structure");
    unlink ("arbre");
}

/*
   Lance la fenêtre gnuplot.
   Commence par créer un pipeline.
   Exécute un fork: le père écrit au fils via le pipeline.
   Le fils connecte la sortie du pipeline sur son entrée standard
	puis disparaît en exécutant gnuplot.
*/

static void start_one_gnuplot (int* gnuplot, char* geometry, char* title)
{   pid_t process;
    int fd [2];

    pipe (fd);
    process = fork ();
    if (process == 0)
    {   close (fd [1]);
        dup2 (fd [0], 0);
        execlp ("gnuplot", "gnuplot", 
			"-geometry", geometry, "-title", title, (char*)0);
        exit (0);
    } else
        close (fd [0]);
    *gnuplot = fd [1];
}

void start_gnuplot (enum algorithme algo, int nb_fenetres)
{   char* title;

    gnuplot1 = PAS_OUVERT;
    gnuplot2 = PAS_OUVERT;
    if (nb_fenetres == 1)
	start_one_gnuplot (&gnuplot1, GNUPLOT_GEOM, "le graphe");
    else
    {	start_one_gnuplot (&gnuplot1, GNUPLOT_GEOM1, "le graphe");
	switch (algo)
	{   case largeur:
	    case profondeur:
	    case Dijkstra:
	    case niveaux:
		title = "l'arbre des predecesseurs";
		break;
	    case Kruskal:
		title = "la relation d'equivalence";
		break;
	    default:
		title = "pas de titre";
		break;
	}
	start_one_gnuplot (&gnuplot2, GNUPLOT_GEOM2, title);
    }
}

void stop_gnuplot ()
{
    gnuplot_printf (gnuplot1, "quit\n");
    gnuplot_printf (gnuplot2, "quit\n");
}

