CXX =c++
CXXFLAGS =-Wall -Wextra -Werror -std=c++98
NAME =webserv
SRC=main.cpp Server/Socket/Server.cpp Server/Socket/Serverupload.cpp Server/Request/Request.cpp Server/Response/Response.cpp Server/Response/Response_Methods.cpp Server/Socket/Server_Cookies.cpp \
	Server/Cgi/Cgi_handler.cpp Config/Config.cpp Server/Socket/Server_utils.cpp Server/Response/Response_utils.cpp
RM=rm -rf

OBJ = $(SRC:.cpp=.o)
all : $(NAME)

$(NAME) :$(OBJ)
	$(CXX) $(CXXFLAGS)  $(OBJ) -o $(NAME)

clean :
	$(RM) $(OBJ)

fclean : clean
	$(RM) $(NAME)

re: fclean $(NAME)