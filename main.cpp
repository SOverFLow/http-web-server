#include "Server/Socket/Server.hpp"
#include "Server/Response/Response.hpp"
#include "Server/Request/Request.hpp"



int main(int argc, char **argv, char **env)
{
    (void)argc;
    (void)argv;
    Server server(env,90,20000, 10);
    server.client_connect();

    Server server2(env,60, 20000, 10);
    server2.client_connect();
    return (0);
}