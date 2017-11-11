#include "un_graphe.h"

/*
   Graphe orienté sans circuit pour algorithme de calcul de niveaux
*/

int B [n][n] = {
    { 0, 1, 0, 1, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 1, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 1, 0, 0, 0, 0, 0, 0 } };

struct vertex S [n] = {
    { 8, 15, vert },
    { 3, 15, vert },
    { 3, 10, vert },
    { 8, 10, vert },
    { 13, 15, vert },
    { 13, 10, vert },
    { 18, 15, vert },
    { 18, 10, vert },
    { 10,  5, vert } };
