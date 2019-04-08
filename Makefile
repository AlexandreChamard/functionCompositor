
CC			=	g++

RM			=	rm -f

CPPFLAGS	=	-W -Wextra -std=c++1z -g

NAME		=	a.out

SRCS		=	srcs/main.cpp	\


OBJS		=	$(SRCS:.cpp=.o)


all			:	$(NAME)

$(NAME)		:	$(OBJS)
				$(CC) -o $(NAME) $(SRCS) $(CPPFLAGS)

clean		:
				$(RM) $(OBJS)

fclean		:	clean
				$(RM) $(NAME)

re			:	fclean all