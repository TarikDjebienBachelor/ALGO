#ifndef FILE_H
#define FILE_H 1

#include <stdbool.h>
#include "def_graphe.h"
#include "un_graphe.h"

struct file {
    sommet tab [n];
    int debut, fin;
};

extern void vider_file (struct file* file);
extern bool est_vide_file (struct file* file);
extern void enfiler_file (sommet s, struct file* file);
extern sommet defiler_file (struct file* file);

#endif /* !FILE_H */

