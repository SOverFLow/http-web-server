#include <map>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>


class Config
{

public:
    Config();
    void parse_config();
    std::map<std::string, std::string> config;
};

