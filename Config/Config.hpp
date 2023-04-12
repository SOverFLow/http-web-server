#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <vector>
#include <map>
#include <string>

class ServerConfig
{
    public:
        std::string root;
        bool        autoindex;
        bool        uploadable;
        bool        redirect;
        bool        CgiStatus;
        int         port;
        int         redirect_code;
        std::string uploadPath;
        std::string redirect_url;
        std::string host;
        std::vector<std::string> server_name;
        size_t client_max_body_size;
        std::vector<std::string> index;
        std::vector<std::string> allowed_method;
        std::map<std::string, std::string> error_pages;
        std::string directory_answer;
        std::string CgiLang;
        std::string CgiPath;
        ServerConfig();
        ~ServerConfig();
    private:

};

class Locations : public ServerConfig
{
    public:
        std::string Name;
        int port;
        Locations( std::string Name );
        ~Locations();

    private:
    
};

class ServerBlock : public ServerConfig
{
    public:
        std::vector<Locations> Locations;
        int sock_fd;
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