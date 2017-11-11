set VEG; /* ensemble pour les huiles VEG1 VEG2 */
set ANI; /* ensemble pour les huiles ANI1 ANI2 ANI3 */
set HUILE := VEG union ANI; /* une ensemble pour l'union de deux ensembles precedent */

/* quantites maximums qu'on peut utiliser pour rafinner la nouriture */
param raf_max_veg >=0; /* pour les huile qui appartien a l'ensemble VEG */
param raf_max_ani >=0; /* pour les huile qui appartien a l'ensemble ANI */

/* le nombres des mois */
param N integer >=0;

/* les duretes de chaques huiles */
param durete {HUILE};
/* durete minimum de la moyenne : pour le DM c'est 3*/
param durete_min;
/* durete maximum de la moyenne : pour le DM c'est 6 */
param durete_max;

/*prix nouriture*/
param prix_nouriture;

/* stockage maximum pour un mois */
param st_max;
/*stock initial*/
param st_initial;
/*stock final*/
param st_final;

/*production minimale*/
param prod_min;

/*melange max des huiles*/
param melange_max;

/* ensemble pour les MOIS : 1 -> Janvier 2-> Fevrier ... n-> ... 12-> Decembre*/
set MOIS := 1 .. N;

/* les prix mensuels des huiles par tonne*/
param prix_mensuel{MOIS,HUILE} >=0;

/* le prix mensuel de stockage par tonne */
param prix_stockage_mensuel;

/* ici il y a des parametres pour l'inflation : par exemple pour %4x -> cst_inflation=4 taux_inflation=x=REEL*/
/* parametre pour constante d'inflation */
param cst_inflation {MOIS,HUILE};
/* parametre pour taux d'inflation */
param taux_inflation;

/* parametre pour les quantites qu'on utilise pour produire la nouriture : pour chaque type- tonne / mois */
var qte_utilise {MOIS,HUILE} >=0;
/* parametre pour les quantites qu'on achete pour la nouriture : pour chaque type- tonne / mois */
var qte_achte {MOIS,HUILE} >=0;

/* parametre pour les quantites des huiles qu'on stocke : pour chaque type- tonne / mois */
var qte_stock {1 .. N+1, HUILE} >=0;

/* ici dans cette tableau on garde les 1s et 0s; 1 si on utilise une type d'huile 0 sinon */
var est_utilise {MOIS,HUILE} binary;

var det {MOIS} integer;
var ana {MOIS} integer;

/* EQUATION OBJECTIVE SIMPLE
voici equation de objective pour Janvier

	x1 : VEG1, x2 : VEG2, x3 : ANI1, x4 : ANI2, x5 : ANI3 ;
	ps : prix stockage mensuel, y1 : stock pour VEG1 ......y5 : stock pour ANI3 ;

	150*(x1+x2+x3+x4+x5) - (110*x1 + 120*x2 + 130*x3 + 110*x4 + 111*x5) - ps*(y1+y2+y3+y4+y5) = z[max]
*/
/* INFLATION
on a ajoute la taux d'inflation a l'equation du profit avec cette phrase :
	prix_mensuel [m,h] * ( (100 + cst_inflation [m,h] * taux_inflation)/100 ) * qte_achte [m,h]

	pour les huile q'on achete on paie prix_mensuel[m,h]*qte_achte[m,h] chaque mois et d'ans le question 3 il dit que ces prix peuvent augmenter avec %x,%2x etc.. ici on nomme la constante cst_inflation et x taux_inflation => inflation = (cst_inflation * taux_inflation)/100. Donc les prix va augmenter comme :  ( prix_mensuel[m,h]*cst_inflation [m,h] * taux_inflation )/100. Alors on a :

	Prix Mensuel = prix_mensuel[m,h] + ( prix_mensuel[m,h]*cst_inflation [m,h] * taux_inflation )/100
		     = prix_mensuel [m,h] * ( (100 + cst_inflation [m,h] * taux_inflation)/100 )
on ajoute cette solution a la profit
*/
maximize profit:
	sum {m in MOIS,h in HUILE} (prix_nouriture * qte_utilise [m,h] - prix_mensuel [m,h] * ( (100 + cst_inflation [m,h] * taux_inflation)/100 ) * qte_achte [m,h] - prix_stockage_mensuel*qte_stock [m,h]);

/*
limitation pour les huiles vegetales
	x1 + x2 <= 200
*/
subject to utilisation_limite_veg_max {m in MOIS}:
	sum {v in VEG} qte_utilise [m,v] <= raf_max_veg;

/*
limitation pour les huiles vegetales
	x3 + x4 + x5 <= 250
*/
subject to utilisation_limite_ani_max {m in MOIS}:
	sum {a in ANI} qte_utilise [m,a] <= raf_max_ani;

/* constrainte pour la durete:
	durete minimum <= { somme [(les quantites de huile) * (durete de huile) ] } / [somme (les quantites des huiles) ] <= durete maxiimum
	=>
	(durete minimum)*[somme (les quantites des huiles) ] <= somme [(les quantites de huile) * (durete de huile) ]
								somme [(les quantites de huile) * (durete de huile) ] <= (durete maxiimum)*[somme (les quantites des huiles) ]
*/
subject to durete_final_min {m in MOIS}:
	sum {h in HUILE} (qte_utilise [m,h] * durete [h]) >= sum {h in HUILE} (durete_min*qte_utilise [m,h]);

subject to durete_final_max {m in MOIS}:
	sum {h in HUILE} (qte_utilise [m,h] * durete [h]) <= sum {h in HUILE} (durete_max*qte_utilise [m,h]);

/* ce qu'il y a dans le stock au debut */
subject to stock_initial {h in HUILE} :
	qte_stock [1,h] = st_initial;

/* ce qu'il y doit avoir dans le stock au final */
subject to stock_final {h in HUILE} :
	qte_stock [N+1,h] = st_final;

/* limite pour stocker les huiles */
subject to stock_max {m in MOIS, h in HUILE} :
	qte_stock [m,h] <= st_max;

/*
equation pour equlibre les stocks avec les achtee et vendus : pour chaque huile
	(qte stock dans cette mois) + (qte achete dans cette mois) = (qte stock de la mois prochaine) + (qte vendu de cette mois)
*/
subject to equilibre {m in MOIS,h in HUILE} :
	qte_stock [m,h] + qte_achte [m,h] = qte_stock [m+1,h] + qte_utilise [m,h];


/* PARTIE POUR LES VARIANTES */

/* VARIANTE 1 */
/* si une type d'huile utilies dans une mois est_utilise =1 sinon =0 */
subject to limitations_veg_m {m in MOIS,h in VEG} :
	qte_utilise [m,h] - raf_max_veg * est_utilise [m,h] <= 0;

/* si une type d'huile utilies dans une mois c'est =1 sinon =0 */
subject to limitations_ani_m {m in MOIS,a in ANI} :
	qte_utilise [m,a] - raf_max_ani * est_utilise [m,a] <= 0;

/* somme des 1 dans une mois doit etre plus petit q'une limite */
subject to limite_a_utilise {m in MOIS}:
	sum {h in HUILE} est_utilise [m,h] <= melange_max;

/* VARIANTE 2 */
/* si on utilise une type d'huile au moins 20 tonnes */
subject to limitations_veg_n {m in MOIS,h in VEG} :
	qte_utilise [m,h] - prod_min * est_utilise [m,h] >= 0;

/* si on utilise une type d'huile au moin 20 tonnes */
subject to limitations_ani_n {m in MOIS,a in ANI} :
	qte_utilise [m,a] - prod_min * est_utilise [m,a] >= 0;


/* VARIANTE 3 */
/* quand on utilise veg1 et veg2 on doit utiliser ani3*/
/*
ici notre equation est : pour chaque mois
	(somme des 1 des huiles vegetales) <= 2*(huile ANI3)
c'est a dire si VEG1+VEG2=2 alors ANI=1
*/

/* dans ce constrainte on choisi le huile grace a son durete (ici c'est5 pour ANI3) apres je somme les huile vegetales et je constuire mon in egalite */
subject to si_veg1_et_veg2_alors_ani3 {m in MOIS,a in ANI} :
	2*est_utilise [m,a] >= if (durete [a]=5) then
					prod{v in VEG} est_utilise[m,v];

data;

set VEG := VEG1 VEG2;
set ANI := ANI1 ANI2 ANI3;

param N := 6;

param prix_nouriture := 150;

param prix_mensuel :
	VEG1	VEG2	ANI1	ANI2	ANI3 :=
1	110	120	130	110	115
2	130	130	110	90	115
3	110	140	130	100	95
4	120	110	120	120	125
5	100	120	150	110	105
6	90	100	140	80	135;

param raf_max_veg := 200;
param raf_max_ani := 250;

param durete_min := 3;
param durete_max := 6;
param durete :=
VEG1	8.8
VEG2	6.1
ANI1	2.0
ANI2	4.2
ANI3 	5.0;

param st_max := 1000;
param st_initial := 500;
param st_final := 500;

param cst_inflation :
	VEG1	VEG2	ANI1	ANI2	ANI3 :=
1	0	0	0	0	0
2	1	1	2	2	2
3	2	2	4	4	4
4	3	3	6	6	6
5	4	4	8	8	8
6	5	5	10	10	10;

param prix_stockage_mensuel := 5;

param taux_inflation := 5;

param prod_min := 20;
param melange_max := 3;
