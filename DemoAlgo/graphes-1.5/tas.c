#include "tas.h"

/**********************************************************************
 IMPLANTATION D'UN TAS DE SOMMETS

 Tout sommet x est inférieur à ses fils gauche et droit.
 Ce sont les valeurs de pi qui sont comparées.

 Utilisé par l'algorithme de Dijkstra.
 **********************************************************************/

void vider_tas (struct tas* tas)
{
    tas->fin = 0;
}

bool est_vide_tas (struct tas* tas)
{
    return tas->fin == 0;
}

/*
   Extrait le sommet du tas dont la valeur de pi est minimale.
   Ce sommet se situe en tas->tab [0]. Reconstruit le tas après extraction.
*/

sommet extraire_tas (struct tas* tas)
{   sommet s, x, y, z;
    int p;

/*
   Le premier sommet du tas est extrait. C'est lui qui sera retourné.
   Un « trou » s'est crée en tas->tab [0].
*/
    x = tas->tab [0];
    s = tas->tab [--tas->fin];
    p = 0;
/*
   On veut combler le « trou ». Le dernier sommet du tas est retiré et rangé 
   dans s. On cherche l'indice p auquel le ranger. Initialement p vaut 0.

   Invariant de boucle : tas->tab [p] est une case libre.

   Tant que s est supérieur à l'un des fils de tas->tab [p], on déplace
   le plus petit des deux fils en tas->tab [p] et on affecte à p l'indice du
   fils déplacé. Parallèlement, on garde tas->pos à jour.

   Quand la boucle s'arrête, il n'y a plus qu'à ranger s dans tas->tab [p].
*/
    for (;;)
    {	if (2*p + 1 >= tas->fin)
	    break;
	y = tas->tab [2*p + 1];
	if (S [y].pi < S [s].pi)
	{   if (2*p + 2 < tas->fin && S [z = tas->tab [2*p + 2]].pi < S [y].pi)
	    {   tas->tab [p] = z;
		tas->pos [z] = p;
		p = 2*p + 2;
	    } else
	    {	tas->tab [p] = y;
		tas->pos [y] = p;
		p = 2*p + 1;
	    }
	} else 
	    if (2*p + 2 < tas->fin && S [z = tas->tab [2*p + 2]].pi < S [s].pi)
	{   tas->tab [p] = z;
	    tas->pos [z] = p;
	    p = 2*p + 2;
	} else
	    break;
    }
    tas->tab [p] = s;
    tas->pos [s] = p;
    return x;
}

/*
    Sous-fonction de inserer_tas et de pousser_tas.

    Hypothèses : le sommet s n'appartient pas à tas->tab [0 .. p0-1].
		 l'emplacement tas->tab [p0] est libre.
    Action     : insère s dans tas->tab [0 .. p0].
*/

static void pousser_aux_tas (sommet s, struct tas* tas, int p0)
{   sommet x;
    int p;

    p = p0;
/*
   On cherche l'indice p où ranger s. Initialement, p = p0.

   Invariant de boucle : tas->tab [p] est libre.

   Tant que s est inférieur à son père, on déplace le père en tas->tab [p]
   et on donne à p l'indice du père. On garde le tableau tas->pos à jour.

   Quand la boucle s'arrête, il n'y a plus qu'à ranger s dans tas->tab [p].
*/
    for (;;)
    {	if (p == 0)
	    break;
	x = tas->tab [p / 2];
	if (S [s].pi >= S [x].pi)
	    break;
	tas->tab [p] = x;
	tas->pos [x] = p;
	p = p / 2;
    }
    tas->tab [p] = s;
    tas->pos [s] = p;
}

/*
   Insère le sommet s dans le tas.
   Le sommet s est supposé ne pas déjà appartenir au tas.
*/

void inserer_tas (sommet s, struct tas* tas)
{
    pousser_aux_tas (s, tas, tas->fin++);
}

/*
    Le sommet s appartient déjà au tas. Il se trouve à l'indice tas->pos [s].
    Sa valeur de pi a été diminuée. Il faut mettre à jour sa position.
*/

void pousser_tas (sommet s, struct tas* tas)
{
    pousser_aux_tas (s, tas, tas->pos [s]);
}

