#include "file.h"

/**********************************************************************
 IMPLANTATION D'UNE FILE
 **********************************************************************/

void vider_file (struct file* file)
{
    file->debut = file->fin = 0;
}

bool est_vide_file (struct file* file)
{
    return file->debut == file->fin;
}

void enfiler_file (sommet s, struct file* file)
{
    file->tab [file->fin] = s;
    file->fin = (file->fin + 1) % n;
}

sommet defiler_file (struct file* file)
{   sommet s;

    s = file->tab [file->debut];
    file->debut = (file->debut + 1) % n;
    return s;
}

