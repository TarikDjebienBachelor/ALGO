/**********************************************************************
 *  parcoursEnProfondeur.c
 *
 *  (Djebien Tarik)  <tarik.djebien@etudiant.univ-lille1.fr>
 *
 *  PROCEDURE en_profondeur(G,s)
 *  Parcours en profondeur d'abord de la composante connexe s,
 *  sans tenir compte des orientations. (VERSION EFFICACE)
 *  DEBUT
 *    POUR tout sommet x FAIRE
 *      colorier x en bleu
 *      i(x):= 0
 *    FINPOUR
 *    vider la pile
 *    colorier s en vert et l'empiler
 *    TANT QUE la pile n'est pas vide FAIRE
 *      x:= le sommet de la pile
 *      i(x):= i(x)+1
 *      trouvé := FAUX
 *      TANT QUE i(x) <= |succ(x)| et non(trouvé) FAIRE
 *        SI succ(i(x),x) est bleu alors
 *           trouvé := VRAI
 *        SINON
 *           i(x):= i(x)+1
 *        FINSI
 *      FINTANTQUE
 *      SI trouvé ALORS
 *        colorier succ(i(x),x) en vert et l'empiler
 *      SINON
 *        colorier x en rouge
 *        depiler x
 *      FINSI
 *    FINTQ
 *  FINPROCEDURE
 ***********************************************************************/

/* Ce programme prend un nom de graphe en entrée et un départ,le charge,
   et creer la visualisation du parcours en profondeur du graphe.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys/wait.h" /* necessaire pour la macro WEXITSTATUS */
#include "graphe.h"
#define  VRAI 1
#define  FAUX 0

/* Couleurs */
typedef enum {ROUGE=0, BLEU=1, VERT=2} tCouleur;
typedef tCouleur tTabCouleurs[MAX_SOMMETS];


void graphe2visuCouleurs(tGraphe g,char *outfile,tTabCouleurs t){
  FILE *fic;
  int res;
  char couleur[20];
  char commande[80];
  char dotfile[80]; /* le fichier .dot pour créer le .ps */
  char psfile[80]; /* le fichier .ps pour lancer evince nomFichier.ps */

  tNomSommet origine,destination,nom;
  tNumeroSommet i;
  tArc arc;

  /* On va creer un fichier pour graphviz, dans le fichier "outfile".dot */
  strcpy(dotfile,outfile);
  strcpy(psfile,outfile);
  strcat(dotfile,".dot"); /* extension .dot */
  strcat(psfile,".ps"); /* extension .ps */

  fic = fopen(dotfile,"w"); /* Ouverture du fichier en droit en ecriture */
  if (fic == NULL)
	  halt("Ouverture du fichier %s en ecriture impossible\n.",dotfile);

  if (grapheEstOriente(g))
  fprintf(fic,"digraph{\n");
  else
  fprintf(fic,"graph{\n");

  for(i=0;i<grapheNbSommets(g);i++){
    grapheRecupNomSommet(g,i,nom);

    switch (t[i]){
	  case ROUGE : strcpy(couleur,"red");break;
	  case BLEU  : strcpy(couleur,"blue");break;
	  case VERT  : strcpy(couleur,"green");break;
	  default    : strcpy(couleur,"black");break;
	}
    fprintf(fic,"%s [color=%s]\n",nom,couleur);
  }


  for(i=0; i < grapheNbArcs(g); i++) {
    arc = grapheRecupArcNumero(g,i);
    grapheRecupNomSommet(g,arc.orig,origine);
    grapheRecupNomSommet(g,arc.dest,destination);
    if (grapheEstOriente(g))
      fprintf(fic,"%s -> %s\n",origine,destination);
    else
      fprintf(fic,"%s -- %s\n",origine,destination);
  }
  fprintf(fic,"}");

  fclose(fic); /* Fermeture du fichier */
  sprintf(commande, "dot -Tps %s -o %s", dotfile,psfile);
  res = system(commande);
  if (WEXITSTATUS(res))
	  halt("La commande suivante a echoue :\n %s \n",commande);
}

void
en_profondeur
(tGraphe G,tNomSommet s,char* outfile){

  tNumeroSommet i,x;
  tNomSommet nom;
  tTabCouleurs tab;
  tPileSommets P;
  int trouve;
  int iX[MAX_SOMMETS];

printf("PARCOURS EN PROFONDEUR : \n");
  // POUR tous les sommets.
  for(i=0;i<grapheNbSommets(G);i++){
  // colorier x en bleu
      tab[i]=BLEU;
  // i(x) := 0
    iX[i]=-1;
  }
  graphe2visuCouleurs(G,outfile,tab);
  getchar();
  // vider la pile
  P = pileSommetsAlloue();
  //colorier s en vert et l'empiler.
  i=grapheChercheSommetParNom(G,s);
  tab[i]=VERT;
  graphe2visuCouleurs(G,outfile,tab);
  getchar();

  pileSommetsEmpile(P,i);


  //    TANT QUE la pile n'est pas vide FAIRE
        while (!(pileSommetsEstVide(P))) {
  //      x := le sommet de la pile
          x = pileSommetsTete(P);
          iX[x]= iX[x]+1;
          trouve = FAUX;
  // TANT QUE i(x)<= |succ(x)| et non trouvé FAIRE
          while ((iX[x]< grapheNbVoisinsSommet(G,x)) && !(trouve)){
		     // si succ(i(x),x) est bleu alors
		     if (tab[grapheVoisinSommetNumero(G,x,iX[x])] == BLEU) {
			   trouve = VRAI;
			 }
			 else{
			   iX[x]= iX[x]+1;
			 }
		  }
  // Si trouve alor
    if (trouve == VRAI){
      // colorier succ(i(x),x) en vert et l'empiler
      tab[grapheVoisinSommetNumero(G,x,iX[x])]=VERT;
      graphe2visuCouleurs(G,outfile,tab);
      getchar();
      pileSommetsEmpile(P,grapheVoisinSommetNumero(G,x,iX[x]));
    }
    else{
      tab[x]=ROUGE;
      graphe2visuCouleurs(G,outfile,tab);
      getchar();
      grapheRecupNomSommet(G,x,nom);
      printf(" %s ",nom);
	  x=pileSommetsDepile(P);
	 }
   }

printf("\n");
}

/*
int
sortir_du_labyrinthe
(tGraphe labyrinthe,tNomSommet entree,tNomSommet sortie,char * out){

	int i,j;
	int trouve = FAUX;
	tTabCouleurs tabCouleurs;
	tNomSommet s;
	tNumeroSommet x,y;
	j=0;

    en_profondeur(labyrinthe,entree,out);
    x = 1 + grapheChercheSommetParNom(labyrinthe,entree);
    y = 1 + grapheChercheSommetParNom(labyrinthe,sortie);

    for(i=0;i<grapheNbSommets(labyrinthe);i++){
	  if (profondeur[])
	}
}
*/

int main(int argc, char *argv[]) {

  tGraphe graphe;
  tNomSommet depart;
  char* visu;
  char nomGraphe[80];

  if (argc != 3) {
    halt("Usage : %s <FichierGraphe>.grp <SommetDepart> \n", argv[0]);
  }

  /* On récupere <nomDuGraphe>.grp en parametre */
  strcpy(nomGraphe,argv[1]);
  visu = strtok (nomGraphe,".");

  /* On cree les fichier <nomDuGraphe>.dot et <nomDuGraphe>.ps */
  graphe = grapheAlloue();
  grapheChargeFichier(graphe,  argv[1]);
  strcpy(depart,argv[2]);
  en_profondeur(graphe,depart,visu);
  grapheLibere(graphe);

  exit(EXIT_SUCCESS);
}

