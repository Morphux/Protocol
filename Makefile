NAME =		protocol_tests
CC =		gcc
CFLAGS = 	-Wall -Werror -Wextra -I inc/ -I lib/inc/ -g -O3
LFLAGS = 	-Llib/ -lmorphux
SRCS = 		$(wildcard src/*.c)
OBJS =		$(SRCS:%.c=%.o)

all: $(NAME)

$(NAME): $(OBJS)
	make -C lib/
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LFLAGS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: re fclean clean
