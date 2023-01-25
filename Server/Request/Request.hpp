#ifndef REQUEST_HPP

#define REQUEST_HPP
#include <iostream>
#include <string.h>
#include <map>

class Request
{
    private:
        std::map <std::string, std::string> mime;

        void InitMime(std::map<std::string, std::string> &mime);
        void SetMethod(std::string req);
        void SetContent();
        void SetPath(std::string req);
    public:
        Request(std::string req);
        ~Request();
        std::string Method;
        std::string Content_Type;
        std::string Path;
        bool    is_Cgi;
};

#endif