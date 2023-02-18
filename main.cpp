#include "Server/Socket/Server.hpp"
#include "Server/Response/Response.hpp"
#include "Server/Request/Request.hpp"
#include "Config/Config.hpp"



int main(int argc, char **argv, char **env)
{
    
   
    if (argc == 2)
    {
        int fd = open(argv[1], O_RDONLY);
        if (fd < 0)
        {
            std::cout << "Error : config file not found" << std::endl;
            return 0;
        }
        else
        {
            close(fd);
            Config Test(argv[1]);
            std::cout << Test.Servers[0].Locations[0].root << std::endl;
            Server server(Test);
        }
    }
    else
    {
        std::cout << "Error : the serve need a config file" << std::endl;
    }
    return (0);
}