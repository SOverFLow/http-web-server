#-fsanitize=address -g3  add this later -fsanitize=address -g3
CXX =c++
CXXFLAGS =-Wall -Wextra -Werror -std=c++98 -fsanitize=address -g3
TARGET =webserv
SRC=main.cpp Server/Socket/Server.cpp Server/Socket/Serverupload.cpp Server/Request/Request.cpp Server/Response/Response.cpp Server/Response/Response_Methods.cpp Server/Socket/Server_Cookies.cpp \
	Server/Cgi/Cgi_handler.cpp Config/Config.cpp Server/Socket/Server_utils.cpp Server/Response/Response_utils.cpp
RM=rm -rf

all : $(TARGET)

$(TARGET) :$(SRC)
	$(CXX) $(CXXFLAGS)  $(SRC) -o $(TARGET)

fclean :
	$(RM) $(TARGET)

re: fclean $(TARGET)