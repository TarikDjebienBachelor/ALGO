#ifndef DEF_GRAPHE_H
#define DEF_GRAPHE_H 1

enum couleur { vert, rouge, bleu };

typedef int sommet;
#define	PAS_UN_SOMMET	-1

struct vertex {
    int x, y;           /* coordonnées pour gnuplot */
    int i;		/* utilisé par le parcours en profondeur */
    enum couleur c;     /* utilisé par tous les algos */
    sommet pred;	/* utilisé par plusieurs algos */
    int niv, cpt;       /* utilisé par le calcul des niveaux */
    int pi;             /* utilisé par l'algorithme de Dijkstra */
};

#endif /* !DEF_GRAPHE_H */
