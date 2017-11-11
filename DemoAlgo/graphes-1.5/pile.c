#include "pile.h"

/**********************************************************************
 IMPLANTATION D'UNE PILE
 **********************************************************************/

void vider_pile (struct pile* pile)
{
    pile->top = 0;
}

bool est_vide_pile (struct pile* pile)
{
    return pile->top == 0;
}

void empiler_pile (sommet s, struct pile* pile)
{
    pile->tab [pile->top++] = s;
}

sommet sommet_pile (struct pile* pile)
{
    return pile->tab [pile->top - 1];
}

sommet depiler_pile (struct pile* pile)
{
    return pile->tab [--pile->top];
}

