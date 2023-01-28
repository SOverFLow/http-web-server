#include "Server/Socket/Server.hpp"
#include "Server/Response/Response.hpp"
#include "Server/Request/Request.hpp"



int main(int argc, char **argv, char **env)
{
    (void)argc;
    (void)argv;
    Server server(env);
    server.do_connect();
    return (0);
}