#include "un_graphe.h"

/*
   Graphe orienté sans circuit pour algorithme de calcul de niveaux
*/

int B [n][n] = {
    { 0, 4, 0, 0, 0, 3, 5 },
    { 4, 0, 3, 0, 1, 2, 0 },
    { 0, 3, 0, 6, 3, 0, 0 },
    { 0, 0, 6, 0, 4, 5, 3 },
    { 0, 1, 3, 4, 0, 3, 0 },
    { 3, 2, 0, 5, 3, 0, 4 },
    { 5, 0, 0, 3, 0, 4, 0 } };

struct vertex S [n] = {
    {  5, 15 },
    { 11, 17 },
    { 17, 10 },
    {  8,  5 },
    { 12, 10 },
    {  8, 10 },
    {  1,  8 } };
