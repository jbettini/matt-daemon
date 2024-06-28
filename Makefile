CXXFLAGS	=	-Wall -Wextra -Werror -g -O3 -std=c++20
NAME	=	Matt-daemon
SRC		=	$(wildcard srcs/*.cpp)

OBJ		=	$(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	$(RM) -f $(OBJ)

fclean: clean
	$(RM) -f $(NAME)

re: fclean all

.PHONY: all clean fclean re