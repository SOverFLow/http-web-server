#include "Server/Socket/Server.hpp"
#include "Server/Response/Response.hpp"
#include "Server/Request/Request.hpp"
#include "Config/Config.hpp"



int main(int argc, char **argv, char **env)
{
    (void)argc;
    (void)argv;
    // Server server(env);
    // server.client_connect();
    // std::string said = Cgi_Handler(argv[1], NULL);
    // std::cout << said << std::endl;
    Config Test(argv[1]);
    //Test.Servers[0].root = "this a fuckin test" ;
    std::cout << Test.Servers[0].root << std::endl;
    std::cout << Test.Servers[1].root << std::endl;
    std::cout << Test.Servers[0].directory_answer << std::endl;
    //std::cout << Test.name << std::endl;
    return (0);
}