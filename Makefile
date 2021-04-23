NAME = pars
CC = gcc
FLAGS = -Wall -Wextra -Werror
HEADER = -I/usr/include/libxml2
OPTION_LIB = -lpq -lxml2
SRCS = main.c

OBJS_SRCS = ${SRCS:.c=.o}

all: $(NAME)

$(NAME): $(OBJS_SRCS)
		$(CC) $(FLAGS) $(OBJS_SRCS) $(HEADER) $(OPTION_LIB) -o $(NAME)
clean:
		rm -f $(OBJS_SRCS)
fclean: clean
		rm -f $(NAME)
re: fclean all

%.o: %.c
	$(CC) $(HEADER) -c $< -o $(<:.c=.o)

.PHONY: all clean fclean re