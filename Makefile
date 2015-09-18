# Neal Trischitta #

NAME=tcp
M_NAME=tcpm
SRC=tcp.c
M_SRC=tcpm.c

CFLAGS= -Wall -Wextra -Werror -pedantic

OBJ=$(SRC:.c=.o)
M_OBJ=$(M_SRC:.c=.o)

all: tcp tcpm

tcp: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o $(NAME)

tcpm: $(M_OBJ)
	$(CC) $(M_OBJ) $(CFLAGS) -o $(M_NAME)

clean:
	-$(RM) *~
	-$(RM) \#*
	-$(RM) *.o
	-$(RM) .*.swp
	-$(RM) *.core

fclean: clean
	-$(RM) $(NAME)
	-$(RM) $(M_SRC)

re: fclean all

