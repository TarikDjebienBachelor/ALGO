NAME    =       graph2visu
SRC	=	graph2visu.c graphe.c
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

