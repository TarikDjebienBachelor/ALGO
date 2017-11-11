/**********************************************************************
 * accesDonneesGraphe.c
 *
 *  (Djebien Tarik)  <Tarik.Djebien@etudiant.univ-lille1.fr>
 *
 ***********************************************************************/

/* Ce program prend un nom de graphe en entrée, le charge,
   et affiche :
   -la liste des sommets qui n'ont pas de voisins.
   -la liste des sommets qui ont le plus de voisins.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graphe.h"


/* Affichage */
void grapheAfficheSommetsIsoles(tGraphe graphe) {
  tNomSommet nom;
  tNumeroSommet i;

  /* Affiche la liste des sommets et leur nom */
  printf("Type du graphe : ");
  if (grapheEstOriente(graphe))  printf("oriente");
  else printf("non oriente");
  printf("\n\n");

  printf("Nombre de sommets : %d\n\n", grapheNbSommets(graphe));

  printf("Liste des sommets :\n");
  for(i=0; i < grapheNbSommets(graphe);i++) {
    grapheRecupNomSommet(graphe, i, nom);
    printf("- %s\n", nom);
  }
  printf("\n");


  /* Affichage des sommets isolés */
  if (grapheEstOriente(graphe)) {
    printf("Liste des sommets qui n'ont pas de successeurs ou predecesseurs :\n");
    for(i=0; i < grapheNbSommets(graphe);i++) {
      grapheRecupNomSommet(graphe, i, nom);
      if (grapheNbSuccesseursSommet(graphe,i) == 0 && grapheNbPredecesseursSommet(graphe,i) == 0)
		 printf("- %s\n",nom);
    }
  }
  else
  {
    /* le graphe n'est pas orienté */
    printf("Liste des sommets qui n'ont pas de voisins' :\n");
	for(i=0; i < grapheNbSommets(graphe);i++) {
      grapheRecupNomSommet(graphe, i, nom);
	  if (grapheNbVoisinsSommet(graphe,i) == 0)
		printf("- %s\n",nom);
	  }
  }

}

void grapheAfficheSommetsDegreMax(tGraphe graphe) {
  tNomSommet nom;
  tNumeroSommet i,temp;
  int nbVoisinsMax;
  tPileSommets P;

  /* Affichage des sommets qui ont le plus de voisins */

  P = pileSommetsAlloue();

  printf("Sommet de degre Maximal du graphe :\n");
  nbVoisinsMax = 0;
    /* Pour chaque sommets du graphe faire : */
    for(i=0; i < grapheNbSommets(graphe);i++) {
      /* Si le nombre de voisin du sommet courant est supérieur au plus grands des degres rencontrés */
	  if (grapheNbVoisinsSommet(graphe,i) >= nbVoisinsMax){
	   /* le plus grand degre est celui du sommet courant */
	   nbVoisinsMax = grapheNbVoisinsSommet(graphe,i);
	   /* on empile le sommet obtenu */
	   if (pileSommetsEstVide(P))
	   pileSommetsEmpile(P,i);
	   else
	   if (grapheNbVoisinsSommet(graphe,pileSommetsTete(P)) < grapheNbVoisinsSommet(graphe,i)){
	   temp=pileSommetsDepile(P);
	   pileSommetsEmpile(P,i);
       }
       else
       pileSommetsEmpile(P,i);
	  }
    }
  /* on affiche la liste */
  while (!(pileSommetsEstVide(P))){
  grapheRecupNomSommet(graphe,pileSommetsDepile(P),nom);
  printf("- %s \n",nom);
  }

  pileSommetsLibere(P);
}


int main(int argc, char *argv[]) {

  tGraphe graphe;

  if (argc != 2) {
    halt("Usage : %s FichierGraphe.grp\n", argv[0]);
  }


  graphe = grapheAlloue();

  grapheChargeFichier(graphe,  argv[1]);
  grapheAfficheSommetsIsoles(graphe);
  grapheAfficheSommetsDegreMax(graphe);

  grapheLibere(graphe);

  exit(EXIT_SUCCESS);
}

