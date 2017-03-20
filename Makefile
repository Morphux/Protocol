NAME =		protocol_tests
CC =		gcc
CFLAGS = 	-Wall -Werror -Wextra -Wno-unused-result -I inc/ -I lib/inc/ -I lib/lib/libmorphux/inc/ -g -O3 -std=c99
LFLAGS = 	-Llib/ -lmpm
SRCS = 		$(wildcard src/*.c)
OBJS =		$(SRCS:%.c=%.o)

all: $(NAME)

$(NAME): $(OBJS)
	make -C lib/
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME) $(LFLAGS)

init:
	make -C lib/ init

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: re fclean clean
