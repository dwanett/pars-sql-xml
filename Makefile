NAME_IN = pars_in
NAME_OUT = pars_out
CC = gcc
FLAGS = -Wall -Wextra -Werror
HEADER = -I/usr/include/libxml2
OPTION_LIB = -lpq -lxml2 -luuid
SRCS = utils.c
SRCS_IN = pars_in.c
SRCS_OUT = pars_out.c

OBJS_SRCS_IN = ${SRCS_IN:.c=.o}
OBJS_SRCS_OUT = ${SRCS_OUT:.c=.o}
OBJS_SRCS = ${SRCS:.c=.o}

all: $(NAME_IN) $(NAME_OUT)

in: $(NAME_IN)

out: $(NAME_OUT)

$(NAME_IN): $(OBJS_SRCS_IN) $(OBJS_SRCS)
		$(CC) $(FLAGS) $(OBJS_SRCS_IN) $(OBJS_SRCS) $(HEADER) $(OPTION_LIB) -o $(NAME_IN)

$(NAME_OUT): $(OBJS_SRCS_OUT) $(OBJS_SRCS)
		$(CC) $(FLAGS) $(OBJS_SRCS_OUT) $(OBJS_SRCS) $(HEADER) $(OPTION_LIB) -o $(NAME_OUT)

clean:
		rm -f $(OBJS_SRCS_OUT)
		rm -f $(OBJS_SRCS_IN)
		rm -f  $(OBJS_SRCS)

fclean: clean
		rm -f $(NAME_IN)
		rm -f $(NAME_OUT)

re: fclean all

%.o: %.c
	$(CC) $(HEADER) -c $< -o $(<:.c=.o)

.PHONY: in out clean fclean re
