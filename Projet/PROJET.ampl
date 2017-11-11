set VEG;
set ANI;
set HUILE := VEG union ANI;

param raf_max_veg >=0;
param raf_max_ani >=0;

param N integer >=0;

param durete {HUILE};
param durete_min;
param durete_max;

param st_max;
param st_initial;
param st_final;

set MOIS := 1 .. N;

param prix_mensuel{MOIS,HUILE} >=0;

param prix_stockage_mensuel;

param cst_inflation {MOIS,HUILE};
param taux_inflation;

var qte_utilise {MOIS,HUILE}  >=0;
var qte_achte {MOIS,HUILE}  >=0;
var qte_stock {1 .. N+1, HUILE}  >=0;

maximize profit :
	sum {m in MOIS,h in HUILE} (150*qte_utilise [m,h] - prix_mensuel [m,h] * ( (100 + cst_inflation [m,h] * taux_inflation)/100 ) * qte_achte [m,h] - prix_stockage_mensuel*qte_stock [m,h]);

subject to utilisation_limite_veg_max {m in MOIS}:
	sum {v in VEG} qte_utilise [m,v] <= raf_max_veg; 

subject to utilisation_limite_ani_max {m in MOIS}:
	sum {a in ANI} qte_utilise [m,a] <= raf_max_ani;

subject to durete_final_min {m in MOIS}:
	sum {h in HUILE} (qte_utilise [m,h] * durete [h]) >= sum {h in HUILE} (durete_min*qte_utilise [m,h]);

subject to durete_final_max {m in MOIS}:
	sum {h in HUILE} (qte_utilise [m,h] * durete [h]) <= sum {h in HUILE} (durete_max*qte_utilise [m,h]);

subject to stock_initial {h in HUILE} : 
	qte_stock [1,h] = st_initial;

subject to stock_final {h in HUILE} : 
	qte_stock [N+1,h] = st_final;

subject to stock_max {m in MOIS, h in HUILE} : 
	qte_stock [m,h] <= st_max;

subject to equilibre {m in MOIS,h in HUILE} :
	qte_stock [m,h] + qte_achte [m,h] = qte_stock [m+1,h] + qte_utilise [m,h];

data;

set VEG := VEG1 VEG2;
set ANI := ANI1 ANI2 ANI3;

param N := 6;

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

param taux_inflation := 0;

