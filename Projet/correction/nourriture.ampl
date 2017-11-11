reset;
option solver cplex;
set type huile;
set huile {type_huile} ordered;
set TOUTES_LES_HUILES := union {t in TYPE_HUILE} HUILES[t] ordered;
set MOIS ordered;

param prix_vente_unitaire; 
# en euros par tonne

param qte_raffinable{t in TYPE_HUILE} >=0;
#en tonnes

param prix_achat{TOUTES_LES_HUILES,MOIS} >=0;
#en euros par tonne

param max_qte_stockable >=0;
# en tonnes

param cout_stockage >=0;

param durete_min >=0;
param durete_max >=0;

param durete{h in TOUTES_LES_HUULES} >=0;

param stock_initial>=0;
param stock_final

param prix_initial {TOUTES_LES HUILES,MOIS}>=0;

param pourcentage_daugmentation >=0;
param coeff_multiplicateur{MOIS,TYPE_HUILE}>=0;

param prix_previsionnel{h in toute_les_huiles,m in mois} :=
sum {t in type_huile : h in huiles[t]}
prix_initial[h,m]*(1+coeff_multiplicateur[m,t]*pourcentage_daugmentation


param quota_min >=0;

var stock_intermediaire{toutes_les_huiles,mois} >=0, <= max_qte_stockable;

var qte_achete{toutes_les_huiles,mois} >=0;
var qte_utilisee{toutes_les_huiles,mois} >=0;

var utilisee{toutes_les_huiles,


maximize profit:
sum{h in toutes_les_huiles,m in mois}
((qte_utilisee[h,m]* prix_vente_unitaire)-(qte_achetee[h,m]*prix_previsionnel[h,m])- stock_intermediaire[h,m]*cout_stockage);




subject to limite_raffinage{m in mois,t in type_huile}:
    sum{h in huiles(t]} qte_utilisee[h,m]<= qte_raffinable[t];
    
subject to durete_majoree{m in MOIS}:
(sum {h in toutes_les_huiles} qte_utilisee[h,m]*durete[h])
<=
durete_max*(sum{h in toutes_les_huiles} qte_utilisee[h,m]);
    

subject to etat_stock{ h in toutes_les_huiles, m in mois}:
stock_intermediarie[h,m] = qte_achetee[h,m] - qte_utilisee[h,m] 
+ ( if m = first(mois) then stock_initial else stock_intermediaire[h,

subject to contrainte_presentielle{h in toutes_les_huiles,m in mois} :
utilisee[h,m] >= qte_utilisee(h,m] / (max {t in type_huile} qte_raffinable[t]);

subject to pas_plus_de_n_max_huile{m in mois}:
sum {h in toutes_les_huiles} utilisee[h,m]<= max;

subject to qte_minimale_si_utilisee{h in toutesleshuiles,m in mois}
qte_utilisee[h,m]>= quota_min*utilisee[h,m];

subject to contrainte_dependante_des_donnes{m in mois}:
 utilisee['ani3',m]>= utilisee['veg1',m]+utilisee['veg2',m]-1;
 
 
data;
set mois = janvier fevrier mars avril mai juin;
set type_huile = vegetale animale;
set huiles['animale'] = ani1 ani2 ani3;
set huiles['vegetale']= veg1 veg2;

param prix_vente_unitaire:= 150;
param qte_raffinable := animale 250 vegetale 200;

param stock initial := 500;

param durete_min:=3;
param durete_max:=6;
param cout_stockage:=5;
param max_qte_stockable:=1000;

param prix_initial (tr) : #transposee
              veg1 veg2 ani1 ani2 ani3
janvier
fevrier
mars
avril
mai
juin



param nmax:=3;
param quota_min:=20;
param pourcentage_d_augmentation := 1;
param coeff_multiplicateur (tr) :
janvier fevrier mars avril mai juin :=
vegetale 0 1 2 3 4 5
animale 0 2 4 5 8 10;
