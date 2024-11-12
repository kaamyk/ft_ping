NAME	=	ft_ping

CC		=	gcc

CC_FLAGS =	-Wall -Wextra -Werror -g3

SRCDIR	=	src
SRCS	=	$(SRCDIR)/main.c	$(SRCDIR)/signal.c	$(SRCDIR)/print.c	$(SRCDIR)/utils.c \
			$(SRCDIR)/error.c	$(SRCDIR)/parsing.c
OBJDIR	=	.obj
OBJS	=	$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

$(OBJDIR):
		mkdir $(OBJDIR)

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
		$(CC) $(CC_FLAGS) -c $< -o $@

all:		$(OBJDIR) $(OBJ) $(NAME)

$(NAME):	 $(OBJDIR) $(OBJS)
		$(CC) $(CC_FLAGS) $(OBJS) -lm -o $(NAME)

clean:
		rm -rf .obj

fclean:		clean
		rm -rf $(NAME)

re:		fclean all

.PHONY:	all clean fclean