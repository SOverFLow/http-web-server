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
}

Config::Config( std::string Path )
{
    this->ServerCount = 0;
    ConfigParser( Path );
}

Locations::Locations( std::string Name )
{
    this->Name = Name;
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
            Instance->root = *(++it);
        else if (*it == "index")
            Instance->index.insert(Instance->index.begin(), splited.begin() + 1, splited.end());
        else if (*it == "allowed_method")
            Instance->allowed_method.insert(Instance->allowed_method.begin(), splited.begin() + 1, splited.end());
        else if (*it == "cgi")
        {
            Instance->CgiStatus = true;
            Instance->CgiLang = splited[1];
            Instance->CgiPath = splited[0];
        }
        std::getline(ConfigFile, line);
        splited = ft_split(line);
    }
    return (*Instance);

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
        else if (*it == "listen")
            Instance->port = std::atoi((*(++it)).c_str());
        else if (*it == "index")
            Instance->index.insert(Instance->index.begin(), splited.begin() + 1, splited.end());
        else if (*it == "allowed_method")
            Instance->allowed_method.insert(Instance->allowed_method.begin(), splited.begin() + 1, splited.end());
        else if (*it == "location")
            Instance->Locations.push_back(SetLocation(ConfigFile, line, *(++it)));
        else if (*it == "cgi")
        {
            Instance->CgiStatus = true;
            Instance->CgiLang = splited[1];
            Instance->CgiPath = splited[0];
        }
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