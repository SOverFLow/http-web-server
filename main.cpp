#include "Server/Socket/Server.hpp"
#include "Server/Response/Response.hpp"
#include "Server/Request/Request.hpp"
#include "Config/Config.hpp"



int main(int argc, char **argv)
{
    if (argc > 2)
    {
        std::cout << "Error : number of param is wrong" << std::endl;
        return 0;
    }
    else if (argc == 2)
    {
        std::ifstream file(argv[1], std::ios::binary);
        
        if (file)
        {
            file.close();
            Config config(argv[1]);
            Server server(config);
            
        }
        else
        {
            std::cout << "Error : config file not found" << std::endl;
            return 0;
        }
    }
    else
    {
        std::ifstream file("Config/Default.conf", std::ios::binary);
        if (file)
        {
            file.close();
            Config config("Config/Default.conf");
            Server server(config);
            
        }
        else
        {
            std::cout << "Error : config file not found" << std::endl;
            return 0;   
        }
    }
    return (0);
}