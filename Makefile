NAME	=	ft_ping

CC		=	gcc

CC_FLAGS =	-Wall -Wextra -Werror -g3

SRCDIR	=	src
SRCS	=	$(SRCDIR)/main.c	$(SRCDIR)/signal.c

OBJDIR	=	.obj
OBJS	=	$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

$(OBJDIR):
		mkdir $(OBJDIR)

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
		$(CC) $(CC_FLAGS) -c $< -o $@

all:		$(OBJDIR) $(OBJ) $(NAME)

$(NAME):	 $(OBJDIR) $(OBJS)
		$(CC) $(CC_FLAGS) $(OBJS) -o $(NAME)

clean:
		rm -rf .obj

fclean:		clean
		rm -rf $(NAME)

re:		fclean all

.PHONY:	all clean fclean