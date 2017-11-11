#include "tableau_sommets.h"

/*
   Le tableau se remplit à partir de la fin
*/

void vider_tableau_sommets (struct tableau_sommets* T)
{
    T->deb = n-1;
}

void ajouter_tableau_sommets (sommet x, struct tableau_sommets* T)
{
    T->tab [T->deb] = x;
    T->deb -= 1;
}

