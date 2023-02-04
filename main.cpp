#include "Server/Socket/Server.hpp"
#include "Server/Response/Response.hpp"
#include "Server/Request/Request.hpp"



int main(int argc, char **argv, char **env)
{
    (void)argc;
    (void)argv;
    Server server(env);
    server.client_connect();
    // std::string said = Cgi_Handler(argv[1], NULL);
    // std::cout << said << std::endl;
    return (0);
}