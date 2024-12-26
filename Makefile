CXXFLAGS	=	-Wall -Wextra -Werror -g -O3 -std=c++20 -fPIE
NAME		=	Matt-daemon
SRC		=	$(wildcard srcs/*.cpp)

OBJ		=	$(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
