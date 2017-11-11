/**********************************************************************
 * graph2visu.c
 *
 *  (Djebien Tarik)  <tarik.djebien@etudiant.univ-lille1.fr>
 *
 ***********************************************************************/

/* Ce programme prend un nom de graphe en entrée, le charge,
   et creer la visualisation du graphe en format .ps
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys/wait.h" /* necessaire pour la macro WEXITSTATUS */
#include "graphe.h"

void graphe2visu(tGraphe g,char *outfile){
  FILE *fic;
  int res;
  char commande[80];
  char dotfile[80]; /* le fichier .dot pour créer le .ps */
  char psfile[80]; /* le fichier .ps pour lancer evince nomFichier.ps */

  tNomSommet origine,destination;
  tNumeroSommet i,j,succ;
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

  if (grapheEstOriente(g)){
   for(i=0; i < grapheNbSommets(g);i++) {
    grapheRecupNomSommet(g,i,origine);
    if (grapheNbSuccesseursSommet(g,i) != 0){
      for(j=0; j<grapheNbSuccesseursSommet(g,i); j++) {
	  succ = grapheSuccesseurSommetNumero(g,i,j);
	  grapheRecupNomSommet(g,succ,destination);
	  fprintf(fic,"%s -> %s\n",origine,destination);
      }
	}
   }
   fprintf(fic,"}");
  }
  else{
    for(i=0; i < grapheNbArcs(g); i++) {
    arc = grapheRecupArcNumero(g,i);
    grapheRecupNomSommet(g,arc.orig,origine);
    grapheRecupNomSommet(g,arc.dest,destination);
    fprintf(fic,"%s -- %s\n",origine,destination);
  }
   fprintf(fic,"}");
  }

  fclose(fic); /* Fermeture du fichier */
  sprintf(commande, "dot -Tps %s -o %s", dotfile,psfile);
  res = system(commande);
  if (WEXITSTATUS(res))
	  halt("La commande suivante a echoue :\n %s \n",commande);
}




int main(int argc, char *argv[]) {

  tGraphe graphe;
  char* visu;
  char nomGraphe[80];

  if (argc<2) {
    halt("Usage : %s FichierGraphe\n", argv[0]);
  }

  /* On récupere <nomDuGraphe>.grp en parametre */
  strcpy(nomGraphe,argv[1]);
  visu = strtok (nomGraphe,".");

  /* On cree les fichier <nomDuGraphe>.dot et <nomDuGraphe>.ps */
  graphe = grapheAlloue();
  grapheChargeFichier(graphe,  argv[1]);
  graphe2visu(graphe,visu);
  grapheLibere(graphe);

  exit(EXIT_SUCCESS);
}
