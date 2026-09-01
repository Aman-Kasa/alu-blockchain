CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -I.

SRC = sha256.c \
      ec_create.c \
      ec_to_pub.c \
      ec_from_pub.c \
      ec_save.c \
      ec_load.c \
      ec_sign.c \
      ec_verify.c

OBJ = $(SRC:.c=.o)

NAME = libhblk_crypto.a

.PHONY: all clean oclean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	ar rcs $(NAME) $(OBJ)

clean:
	$(RM) *~ $(NAME)

oclean:
	$(RM) $(OBJ)

fclean: clean oclean

re: fclean all
