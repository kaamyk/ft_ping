NAME	=	ft_ping

CC		=	gcc
CC_FLAGS =	-Wall -Wextra -Werror -g3

SRCDIR	=	src
SRCS	=	$(SRCDIR)/main.c	$(SRCDIR)/signal.c	$(SRCDIR)/print.c	$(SRCDIR)/utils.c \
			$(SRCDIR)/error.c	$(SRCDIR)/parsing.c
OBJDIR	=	.obj
OBJS	=	$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
DEPDIR	=	.dep
DEPS	=	$(patsubst $(SRCDIR)/%.c, $(DEPDIR)/%.d, $(SRCS))

$(OBJDIR):
		@mkdir -v $(OBJDIR)

$(DEPDIR):
		@mkdir -v $(DEPDIR)

$(OBJDIR)/%.o:	$(SRCDIR)/%.c
		@echo -n "Compiling " $< " ... "
		@$(CC) $(CC_FLAGS) -c $< -o $@
		@$(CC) $(CC_FLAGS) -MM $< | sed 's,\($*\)\.o[ :]*,$(DEPDIR)/\1.o $@ : ,g' > $(DEPDIR)/$*.d
		@echo -e '\x1b[32m>>> OK <<<\x1b[37m'

all:		$(OBJDIR) $(DEPDIR) $(OBJ) $(NAME)

$(NAME):	 $(OBJDIR) $(DEPDIR) $(OBJS)
		@echo -e '\033[0;34mObjects compilation: \x1b[32m>>> OK <<<\x1b[37m'
		@echo ""
		@echo  -n "Compiling " $(NAME) " ... "
		@$(CC) $(CC_FLAGS) $(OBJS) -lm -o $(NAME)
		@echo -e '\x1b[32m>>> OK <<<\x1b[37m'
		@echo -en '\033[0;34mExecutable '
		@echo -n  $(NAME)
		@echo -e  ' compiled: \x1b[32m>>> OK <<<\x1b[37m'
		@echo ""

clean:
		@rm -rfv $(OBJDIR) $(DEPDIR)
		@echo -e '\033[0;34mObject/Dependencies files removed: \x1b[32m>>> OK <<<\x1b[37m'
		@echo ""

fclean:		clean
		@rm -rfv $(NAME) $(NAME).d
		@echo -e '\033[0;34mExecutable file removed: \x1b[32m>>> OK <<<\x1b[37m'
		@echo ""

-include $(DEPS)

re:		fclean all

.PHONY:	all clean fclean