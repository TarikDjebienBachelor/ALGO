#include "un_graphe.h"

int B [n][n] = {
    { 0, 2, 1, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 9, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 3, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 7, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 0, 0, 2, 0, 0, 0, 0, 0 } };

struct vertex S [n] = {
    { 8, 15 },
    { 3, 15 },
    { 3, 10 },
    { 8, 10 },
    { 13, 15 },
    { 13, 10 },
    { 18, 15 },
    { 18, 10 },
    { 10,  5 } };
