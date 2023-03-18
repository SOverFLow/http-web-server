#ifndef REQUEST_HPP

#define REQUEST_HPP
#include <iostream>
#include <string.h>
#include <map>
#include "../../Config/Config.hpp"



class Request
{
    private:
        std::map <std::string, std::string> mime;

        void InitMime(std::map<std::string, std::string> &mime);
        void SetMethod(std::string req);
        void SetContent();
        void SetPath(std::string req);
        void SetBody(std::string req);
        void SetHost(std::string req);
    public:
        Request(std::string req, size_t server_body_size);
        int Is_Request_Well_Formed(std::string req);
        ~Request();
        std::string Method;
        int StatusCode;
        int cgiStatus;
        std::string Content_Type;
        size_t Content_Lenght;
        size_t Body_Size_From_Config;
        std::string Body;
        std::string Host;
        std::string Qurey_String;
        std::string Path;
        bool    is_Cgi;
};

std::string     Cgi_Handler(Request &req, std::string Path, char **env, std::string cgiLang, ServerBlock &Server);


#endif