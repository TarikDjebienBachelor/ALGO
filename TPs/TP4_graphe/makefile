NAME=   parcoursEnProfondeur
SRC	=	parcours_en_profondeur.c graphe.c
OBJ	=	$(SRC:.c=.o)
CC	=	gcc
CFLAGS	=	-Wall -Werror -g -std=c99
RM	=	rm -f

all	:	$(OBJ)
		$(CC) -o $(NAME) $(OBJ)
clean	:
		$(RM) *~ *\# \#*
		$(RM) $(OBJ)

fclean	:	clean
		$(RM) $(NAME)

re	:	fclean all
