#include "Server/Socket/Server.hpp"
#include "Server/Response/Response.hpp"
#include "Server/Request/Request.hpp"
#include "Config/Config.hpp"



int main(int argc, char **argv)
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
            Config config(argv[1]);
            Server server(config);
        }
    }
    else
    {
        int fd = open("Config/Default.conf", O_RDONLY);
        if (fd < 0)
        {
            std::cout << "Error : config file not found" << std::endl;
            return 0;
        }
        else
        {
            close(fd);
            Config config("Config/Default.conf");
            Server server(config);
        }
    }
    return (0);
}