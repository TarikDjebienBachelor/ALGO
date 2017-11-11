CC = gcc
CFLAGS = -g -Wall
LDFLAGS = -g
OBJETS = main.o un_graphe.o algos_graphe.o gnuplot.o \
	pile.o file.o tas.o tableau_arcs.o tableau_sommets.o \
	relation_eqv.o 
ALGO = demo_algo
EXEMPLES = acyclique1 circuit_positif1 non_connexe1 \
	reseau_transport1 reseau_transport2 reseau_transport3 \
	non_oriente1 non_oriente2

# $< = premier prerequis
# $^ = tous les prerequis
# $@ = cible

.PHONY: all clean 

all:
	$(MAKE) clean
	$(MAKE) $(ALGO)

# -rm au lieu de rm fait que l'erreur est ignorée (si pas de fichiers)
clean:
	-rm $(ALGO) *.o gnuplot_version.h gnuplot_geometry.h

%.o: %.c gnuplot_version.h gnuplot_geometry.h
	$(CC) -c $(CFLAGS) $<

gnuplot_version.h: 
	./test_gnuplot > gnuplot_version.h

gnuplot_geometry.h:
	./geom_gnuplot > gnuplot_geometry.h

$(ALGO): $(OBJETS)
	$(CC) $(LDFLAGS) $^ -o $@

$(EXEMPLES):
	-rm un_graphe.c un_graphe.h
	cp EXEMPLES/$@.h un_graphe.h
	cp EXEMPLES/$@.c un_graphe.c
	$(MAKE) all

