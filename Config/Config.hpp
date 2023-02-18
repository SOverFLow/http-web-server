#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <map>
#include <string>

class ServerConfig
{
    public:
        std::string root;
        bool        directory_listing;
        int         port;
        std::string host;
        std::string server_names;
        std::string client_max_body_size;
        std::vector<std::string> index;
        std::vector<std::string> allowed_method;
        std::string directory_answer;
        ServerConfig();
        ~ServerConfig();
    private:

};

class Locations : public ServerConfig
{
    public:
    std::string Name;
    Locations( std::string Name );
    ~Locations();

    private:
    
};

class ServerBlock : public ServerConfig
{
    public:
        std::vector<Locations> Locations;
        ServerBlock();
        ~ServerBlock();
    private:
};

class Config
{
    public:
        int ServerCount;
        Config( std::string Path );
        ~Config();
        void    ConfigParser( std::string Path );
        std::vector<ServerBlock> Servers;
};

#endif