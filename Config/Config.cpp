#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <cstring>

ServerConfig::ServerConfig()
{

}

ServerConfig::~ServerConfig()
{

}

ServerBlock::ServerBlock()
{

}

ServerBlock::~ServerBlock()
{
    this->CgiStatus = false;
    this->redirect = false;
    this->uploadable = false;
}

Config::Config( std::string Path )
{
    this->ServerCount = 0;
    ConfigParser( Path );
}

Locations::Locations( std::string Name )
{
    this->Name = Name;
    this->CgiStatus = false;
    this->redirect = false;
}
Locations::~Locations(){}
Config::~Config(){}

std::vector<std::string> ft_split( std::string line )
{
    std::vector<std::string> splited;
    char *str;
    char *s = (char *)line.c_str();
    str = std::strtok(s, " ");
    while (str)
    {
        splited.push_back(str);
        str = std::strtok(NULL, " ");
    }
    return (splited);
}

Locations &SetLocation(std::ifstream &ConfigFile, std::string line, std::string Name)
{
    Locations *Instance = new Locations( Name );
    std::vector<std::string> splited = ft_split(line);
    if (splited.size() == 3)
    {
        if (splited[2] == "{")
            std::getline(ConfigFile, line);
    }
    else
    {
        std::getline(ConfigFile, line);
        splited = ft_split(line);
        if (splited[0] == "{")
            std::getline(ConfigFile, line);
    }
    splited = ft_split(line);
    while (splited[0] != "}")
    {
        std::vector<std::string>::iterator it = splited.begin();
        if (*it == "root")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in root" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->root = *(++it);
        }
        else if (*it == "index")
        {
            if (splited.size() < 2)
            {
                std::cerr << "Config Error in index" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->index.insert(Instance->index.begin(), splited.begin() + 1, splited.end());
        }
        else if (*it == "allowed_method")
        {
            if (splited.size() < 2)
            {
                std::cerr << "Config Error in allowed_method" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->allowed_method.insert(Instance->allowed_method.begin(), splited.begin() + 1, splited.end());
        }
        else if (*it == "cgi")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in cgi" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->CgiStatus = true;
            Instance->CgiLang = splited[1];
        }
        else if (splited[0] == "autoindex")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in autoindex" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (splited[1] == "on")
                Instance->autoindex = true;
            else 
                Instance->autoindex = false;
        }
        else if (splited[0] == "return")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in redirection" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->redirect = true;
            Instance->redirect_code = std::atoi(splited[1].c_str());
            Instance->redirect_url = splited[2];
        }
        else  if (splited[0] == "upload")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error: no upload path" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->uploadable = true;
            Instance->uploadPath = splited[1];
        }
        else if (splited[0] == "error_page")
        {
            if (splited.size() != 3)
            {
                std::cerr << "Config Error in error_page" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->error_pages[splited[1]] = splited[2];
        }
        else
        {
            std::cerr << "Config Error unknown directive" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::getline(ConfigFile, line);
        splited = ft_split(line);
    }
    return (*Instance);

}

ServerBlock &SetServer(std::ifstream &ConfigFile, std::string line)
{
    ServerBlock *Instance = new ServerBlock();
    std::vector<std::string> splited = ft_split(line);

    while (splited[0] != "}")
    {
        if (splited[0] == "root")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in root" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->root = splited[1];
        }
        else if (splited[0] == "directory_answer")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in directory_answer" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->directory_answer = splited[1];
        }
        else if (splited[0] == "listen")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in port" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->port = std::atoi(splited[1].c_str());
        }
        else if (splited[0] == "index")
        {
            if (splited.size() < 2)
            {
                std::cerr << "Config Error in index" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->index.insert(Instance->index.begin(), splited.begin() + 1, splited.end());
        }
        else if (splited[0] == "allowed_method")
        {
            if (splited.size() < 2)
            {
                std::cerr << "Config Error in allowed_method" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->allowed_method.insert(Instance->allowed_method.begin(), splited.begin() + 1, splited.end());
        }
        else if (splited[0] == "location")
            Instance->Locations.push_back(SetLocation(ConfigFile, line, splited[1]));
        else if (splited[0] == "cgi")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in cgi" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->CgiStatus = true;
            Instance->CgiLang = splited[1];
        }
        else if (splited[0] == "autoindex")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in autoindex" << std::endl;
                exit(EXIT_FAILURE);
            }
            if (splited[1] == "on")
                Instance->autoindex = true;
            else 
                Instance->autoindex = false;
        }
        else if (splited[0] == "return")
        {
            if (splited.size() != 3)
            {
                std::cerr << "Config Error in reduction" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->redirect = true;
            Instance->redirect_code = std::atoi(splited[1].c_str());
            Instance->redirect_url = splited[2];
        }
        else if (splited[0] == "server_name")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in server_name" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->server_name = splited[1];
        }
        else if (splited[0] == "client_max_body_size")
        {
            if (splited.size() != 2)
            {
                std::cerr << "Config Error in client_max_body_size" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->client_max_body_size = atoi(splited[1].c_str());
        }
        else if (splited[0] == "error_page")
        {
            if (splited.size() != 3)
            {
                std::cerr << "Config Error in error_page" << std::endl;
                exit(EXIT_FAILURE);
            }
            Instance->error_pages[splited[1]] = splited[2];
        }
        else
        {
            std::cerr << "Config Error unknown directive" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::getline(ConfigFile, line);
        splited = ft_split(line);
        while (splited.size() == 0)
        {
            std::getline(ConfigFile, line);
            splited = ft_split(line);
        }
    }
    return (*Instance);
}

void    Config::ConfigParser( std::string Path )
{
    std::ifstream ConfigFile(Path);
    std::string line;
    std::string token;
    while (std::getline(ConfigFile, line))
    {
        if (line == "server")
        {
            std::getline(ConfigFile, line);
            if(line == "{")
            {
                std::getline(ConfigFile, line);
                this->Servers.push_back(SetServer(ConfigFile, line));
            }
            this->ServerCount++;
        }
    }
}