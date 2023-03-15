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
    try 
    {
        ConfigParser( Path );
    }
    catch(const std::exception &e)
    {
        std::cout << "error => " << e.what() << std::endl;
    }
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
            Instance->root = *(++it);
        else if (*it == "index")
            Instance->index.insert(Instance->index.begin(), splited.begin() + 1, splited.end());
        else if (*it == "allowed_method")
            Instance->allowed_method.insert(Instance->allowed_method.begin(), splited.begin() + 1, splited.end());
        else if (*it == "cgi")
        {
            Instance->CgiStatus = true;
            Instance->CgiLang = splited[1];
            Instance->CgiPath = splited[2];
        }
        else if (splited[0] == "autoindex")
        {
            if (splited[1] == "on")
                Instance->autoindex = true;
            else 
                Instance->autoindex = false;
        }
        else if (splited[0] == "return")
        {
            Instance->redirect = true;
            Instance->redirect_code = std::atoi(splited[1].c_str());
            Instance->redirect_url = splited[2];
        }
        else  if (splited[0] == "upload")
        {
            Instance->uploadable = true;
            Instance->uploadPath = splited[1];
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
            Instance->CgiPath = splited[2];
        }
        else if (splited[0] == "autoindex")
        {
            if (splited[1] == "on")
                Instance->autoindex = true;
            else 
                Instance->autoindex = false;
        }
        else if (splited[0] == "return")
        {
            Instance->redirect = true;
            Instance->redirect_code = std::atoi(splited[1].c_str());
            Instance->redirect_url = splited[2];
        }
        else if (splited[0] == "server_name")
            Instance->server_name = splited[1];
        else if (splited[0] == "client_max_body_size")
            Instance->client_max_body_size = atoi(splited[1].c_str());
        std::getline(ConfigFile, line);
        splited = ft_split(line);
    }
    return (*Instance);
}

void    Config::ConfigParser( std::string Path )
{
    std::ifstream ConfigFile(Path);
    std::string line;
    std::string token;
    ServerBlock srv;
    while (std::getline(ConfigFile, line))
    {
        if (line == "server")
        {
            std::getline(ConfigFile, line);
            if(line == "{")
            {
                std::getline(ConfigFile, line);
                try 
                {
                    srv = SetServer(ConfigFile, line);
                }
                catch(const std::exception &e)
                {
                    std::cout << "error=> " << e.what() << std::endl;
                }
                this->Servers.push_back(srv);
            }
            this->ServerCount++;
        }
    }
}