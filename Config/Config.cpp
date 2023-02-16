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

}

Config::Config( std::string Path )
{
    this->ServerCount = 0;
    ConfigParser( Path );
}

Locations::Locations(){}
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

ServerBlock &SetServer(std::ifstream &ConfigFile, std::string line)
{
    ServerBlock *Instance = new ServerBlock();
    std::string token;
    while (line != "}")
    {
        std::vector<std::string> splited = ft_split(line);
        std::vector<std::string>::iterator it = splited.begin();
        if (*it == "root")
            Instance->root = *(++it);
        else if (*it == "directory_answer")
            Instance->directory_answer = *(++it);
        std::getline(ConfigFile, line);
    }
    return (*Instance);
}

void    Config::ConfigParser( std::string Path )
{
    std::ifstream ConfigFile(Path);
    std::string line;
    std::string token;
    ServerBlock srv;
    char *str;
    while (std::getline(ConfigFile, line))
    {
        if (line == "server")
        {
            std::getline(ConfigFile, line);
            if(line == "{")
            {
                std::getline(ConfigFile, line);
                srv = SetServer(ConfigFile, line);
                this->Servers.push_back(srv);
            }
            this->ServerCount++;
        }
    }
}