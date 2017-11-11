/**********************************************************************
 *  parcoursEnLargeur.c
 *
 *  (Djebien Tarik)  <tarik.djebien@etudiant.univ-lille1.fr>
 *
 *  PROCEDURE plus_courte_chaine(G,s)
 *  Plus courte chaine entre s et tout autre sommet,
 *  sans tenir compte des orientations.
 *  DEBUT
 *    colorier en bleu tous les sommets sauf s.
 *    vider la file.
 *    d(s) := 0
 *    pred(s) est indéfini
 *    colorier s en vert et l'enfiler.
 *    TANT QUE la file n'est pas vide FAIRE
 *      defiler x
 *      POUR TOUT voisin bleu y de x FAIRE
 *           colorier y en vert et l'enfiler
 *           d(y):= d(x) +1
 *           pred(y):=x
 *      FINPOUR
 *      colorier x en rouge
 *    FINTQ
 *  FINPROCEDURE
 ***********************************************************************/

/* Ce programme prend un nom de graphe en entrée et un départ,le charge,
   et creer la visualisation du parcours en largeur du graphe.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sys/wait.h" /* necessaire pour la macro WEXITSTATUS */
#include "graphe.h"


/* Couleurs */
typedef enum {ROUGE=0, BLEU=1, VERT=2} tCouleur;
typedef tCouleur tTabCouleurs[MAX_SOMMETS];

int d[MAX_SOMMETS];
tNumeroSommet pred[MAX_SOMMETS];


void graphe2visuCouleurs(tGraphe g,char *outfile,tTabCouleurs t, tNumeroSommet p[MAX_SOMMETS]){
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
    fprintf(fic,"%s [color=%s];\n",nom,couleur);
  }


  for(i=0; i < grapheNbArcs(g); i++) {
    arc = grapheRecupArcNumero(g,i);
    grapheRecupNomSommet(g,arc.orig,origine);
    grapheRecupNomSommet(g,arc.dest,destination);

    if (grapheEstOriente(g)){
		if ( p[grapheChercheSommetParNom(g,destination)] == grapheChercheSommetParNom(g,origine) )
            fprintf(fic,"%s -> %s\n [color=blue]",origine,destination);
        else
            fprintf(fic,"%s -> %s\n [color=black]",origine,destination);
       }
    else{
	    if ( p[grapheChercheSommetParNom(g,destination)] == grapheChercheSommetParNom(g,origine) )
            fprintf(fic,"%s -- %s\n [color=blue]",origine,destination);
        else
            fprintf(fic,"%s -- %s\n [color=black]",origine,destination);
       }

  }
  fprintf(fic,"}");

  fclose(fic); /* Fermeture du fichier */
  sprintf(commande, "dot -Tps %s -o %s", dotfile,psfile);
  res = system(commande);
  if (WEXITSTATUS(res))
	  halt("La commande suivante a echoue :\n %s \n",commande);
}

void
plus_courte_chaine
(tGraphe G,tNomSommet s, char* outfile){

  tNumeroSommet i,x,y;
  tNomSommet nom,nomX,nomY;
  tTabCouleurs tab;
  tFileSommets F;

printf("PLUS COURTE CHAINE : \n");
  // colorier en bleu tous les sommets sauf s.
  for(i=0;i<grapheNbSommets(G);i++){
  grapheRecupNomSommet(G,i,nom);
    if(strcmp(nom,s)!=0)
      tab[i]=BLEU;
  }
  for(i=0;i<grapheNbSommets(G);i++){
  pred[i]=-1;
  }
  graphe2visuCouleurs(G,outfile,tab,pred);
  getchar();
  // vider la file
  F = fileSommetsAlloue();
  // d(s) := 0
  d[grapheChercheSommetParNom(G,s)]=0;
  // pred(s) indefini
  pred[grapheChercheSommetParNom(G,s)]= -1;
  //colorier s en vert et l'enfiler.
  i=grapheChercheSommetParNom(G,s);
  tab[i]=VERT;
  graphe2visuCouleurs(G,outfile,tab,pred);
  getchar();
  fileSommetsEnfile(F,i);

  if (grapheEstOriente(G)){
  //    TANT QUE la file n'est pas vide FAIRE
        while (!(fileSommetsEstVide(F))) {
  //      defiler x
          x = fileSommetsDefile(F);
          grapheRecupNomSommet(G,x,nomX);
  //      POUR TOUT voisin y de x FAIRE
          for(i=0;i<grapheNbSuccesseursSommet(G,x);i++){
			y = grapheSuccesseurSommetNumero(G,x,i);
            grapheRecupNomSommet(G,y,nomY);
  //        SI y est bleu ALORS
            if (tab[y]==BLEU) {
  //           colorier y en vert et l'enfiler
               tab[y]=VERT;
               graphe2visuCouleurs(G,outfile,tab,pred);
               getchar();
               fileSommetsEnfile(F,y);
               // d(y):= d(x)+1
               d[grapheChercheSommetParNom(G,nomY)]= d[grapheChercheSommetParNom(G,nomX)] + 1;
               // pred(y):= x
               pred[grapheChercheSommetParNom(G,nomY)] = x;
            }
	      }
  //      FINPOUR
  //      colorier x en rouge
          tab[x]= ROUGE;
          graphe2visuCouleurs(G,outfile,tab,pred);
          getchar();
          grapheRecupNomSommet(G,x,nom);
          printf(" %s ",nom);
	    }
  //    FINTQ
  }
  else
  {
  //    TANT QUE la file n'est pas vide FAIRE
        while (!(fileSommetsEstVide(F))) {
  //      defiler x
          x = fileSommetsDefile(F);
          grapheRecupNomSommet(G,x,nomX);
  //      POUR TOUT voisin y de x FAIRE
          for(i=0;i<grapheNbVoisinsSommet(G,x);i++){
			y = grapheVoisinSommetNumero(G,x,i);
			grapheRecupNomSommet(G,y,nomY);
  //        SI y est bleu ALORS
            if (tab[y]==BLEU) {
  //           colorier y en vert et l'enfiler
               tab[y]=VERT;
               graphe2visuCouleurs(G,outfile,tab,pred);
               getchar();
               fileSommetsEnfile(F,y);
                // d(y):= d(x)+1
               d[grapheChercheSommetParNom(G,nomY)]= d[grapheChercheSommetParNom(G,nomX)] + 1;
               // pred(y):= x
               pred[grapheChercheSommetParNom(G,nomY)] = x;
            }
	      }
  //      FINPOUR
  //      colorier x en rouge
          tab[x]= ROUGE;
          graphe2visuCouleurs(G,outfile,tab,pred);
          getchar();
          grapheRecupNomSommet(G,x,nom);
          printf(" %s ",nom);
	    }
  //    FINTQ
  }

printf("\n");
}

void
afficherTab
(int t[],int longueur,char* name,tGraphe G){
int i;
tNomSommet nom;
 printf("%s:= [ ",name);
 for(i=0;i<grapheNbSommets(G);i++){
	 if (t[i]==-1)
	 printf("indéfini ");
	 else {
		 grapheRecupNomSommet(G,t[i],nom);
		 if (strcmp(name,"d")==0)
		 printf("%s(%d) ",nom,t[i]);
		 else
		 printf("%s ",nom);
		 }
	 }
 printf(" ]\n");
}

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
  plus_courte_chaine(graphe,depart,visu);
  afficherTab(d,grapheNbSommets(graphe),"d",graphe);
  afficherTab(pred,grapheNbSommets(graphe),"pred",graphe);
  grapheLibere(graphe);

  exit(EXIT_SUCCESS);
}

