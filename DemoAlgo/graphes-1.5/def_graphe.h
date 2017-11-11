#ifndef DEF_GRAPHE_H
#define DEF_GRAPHE_H 1

enum couleur { vert, rouge, bleu };

typedef int sommet;
#define	PAS_UN_SOMMET	-1

struct vertex {
    int x, y;           /* coordonn�es pour gnuplot */
    int i;		/* utilis� par le parcours en profondeur */
    enum couleur c;     /* utilis� par tous les algos */
    sommet pred;	/* utilis� par plusieurs algos */
    int niv, cpt;       /* utilis� par le calcul des niveaux */
    int pi;             /* utilis� par l'algorithme de Dijkstra */
};

#endif /* !DEF_GRAPHE_H */
