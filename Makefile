SRC			=	scop.cpp
GLAD_SRC	=	glad/src/glad.c
NAME		=	scop
OBJ			=	$(SRC:.cpp=.o)
GLAD_OBJ	=	$(GLAD_SRC:.c=.o)
CFLAGS		=	-Wextra -std=c++17 -lglfw
INCLUDE_DIR = 	"glad/include"

%.o :		%.cpp
			g++ -c -g -I$(INCLUDE_DIR) $(CFLAGS) $< -o $@

%.o :		%.c
			g++ -c -g -I$(INCLUDE_DIR) $< -o $@

all:		$(NAME)

$(NAME):	$(OBJ) $(GLAD_OBJ)
			g++ $(OBJ) $(GLAD_OBJ) $(CFLAGS) -I$(INCLUDE_DIR) -o $(NAME)

clean:
			rm -f $(OBJ) $(GLAD_OBJ)

fclean:		clean
			rm -f $(NAME)

re:			fclean all

.PHONY:		all clean fclean re
