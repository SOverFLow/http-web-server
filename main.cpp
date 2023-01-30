#include "Server/Socket/Server.hpp"
#include "Server/Response/Response.hpp"
#include "Server/Request/Request.hpp"



int main(int argc, char **argv, char **env)
{
    (void)argc;
    (void)argv;
    char c;
    int fd = open("out.txt", O_RDWR | O_CREAT, 0666);
    Server server(env);
    //Cgi_Handler(argv[1], env, fd);
    //std::cout << said << std::endl;
    // while(read(fd, &c, 1))
    //     std::cout << c << std::endl;
    //server.do_connect();
    return (0);
}