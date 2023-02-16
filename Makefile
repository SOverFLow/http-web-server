CXX =c++
CXXFLAGS =-Wall -Wextra -Werror -std=c++98
TARGET =webserv
SRC=main.cpp Server/Socket/Server.cpp Server/Socket/Serverupload.cpp Server/Request/Request.cpp Server/Response/Response.cpp Server/Response/Response_Methods.cpp Server/Response/Cookies_Session.cpp \
	Server/Cgi/Cgi_handler.cpp Config/Config.cpp
RM=rm -rf

all : $(TARGET)

$(TARGET) :$(SRC)
	$(CXX)  $(SRC) -o $(TARGET)

fclean :
	$(RM) $(TARGET)

re: fclean $(TARGET)