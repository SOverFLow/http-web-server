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
        void SetBody(std::string req);
    public:
        Request(std::string req);
        int Is_Request_Well_Formed(std::string req);
        ~Request();
        std::string Method;
        int StatusCode;
        std::string Content_Type;
        size_t Content_Lenght;
        std::string Body;
        std::string Host;
        std::string Qurey_string;
        std::string Path;
        bool    is_Cgi;
};

std::string     Cgi_Handler(std::string path, char **env);


#endif