#include "un_graphe.h"

/*
   Graphe orient� sans circuit pour algorithme de calcul de niveaux
*/

int B [n][n] = {
/*    a  b  c  d  e  f  g  h  i */
    { 0, 4, 0, 0, 0, 0, 0, 8, 0 },
    { 4, 0, 8, 0, 0, 0, 0, 11, 0 },
    { 0, 8, 0, 7, 0, 4, 0, 0, 2 },
    { 0, 0, 7, 0, 9, 14, 0, 0, 0 },
    { 0, 0, 0, 9, 0, 10, 0, 0, 0 },
    { 0, 0, 4, 14, 10, 0, 2, 0, 0 },
    { 0, 0, 0, 0, 0, 2, 0, 1, 6 },
    { 8, 11, 0, 0, 0, 0, 1, 0, 7 },
    { 0, 0, 2, 0, 0, 0, 6, 7, 0 } };

struct vertex S [n] = {
    {  5, 10 },
    {  8, 15 },
    { 12, 17 },
    { 15, 15 },
    { 17, 10 },
    { 15, 5 },
    { 12, 3 },
    { 8, 5 },
    { 10, 10 } };