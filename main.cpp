#include "Server/Socket/Server.hpp"
#include "Server/Response/Response.hpp"
#include "Server/Request/Request.hpp"



int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    Server server;
    server.do_connect();
    return (0);
}