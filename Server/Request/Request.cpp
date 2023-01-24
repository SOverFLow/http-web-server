#include "Request.hpp"

Request::Request(std::string req)
{

}

Request::~Request()
{

}

void Request::SetMethod(std::string req)
{
    if (!strncmp(&req[0], "GET", 3))
        this->Method = "GET";
    else if (!strncmp(&req[0], "POST", 4))
        this->Method = "POST";
    else if (!strncmp(&req[0], "DELETE", 6))
        this->Method = "DELETE";
    else
        this->Method = "NULL";
}

void Request::SetPath(std::string req)
{
    int start = this->Method.length() + 1;
    int len = 0;
    for(int i = start; req[i]; i++)
    {
        if (req[i] != ' ')
            len++;
    }
    this->Path = req.substr(start, len);
}

std::string GetMime(std::string Path)
{
    // some code.
}

void Request::SetContent(std::string req)
{
    std::map<std::string, std::string>::iterator it;
    std::string tmp = GetMime(this->Path);
    char *type = &tmp[0];
    it = this->mime.begin();
    while (it != this->mime.end())
    {
        if (strcmp(type, &it->first[0]))
        {
            this->Content_Type = it->second;
                break;
        }
        it++;
    }
}

void Request::InitMime(std::map<std::string, std::string> &mime)
{
    mime["html"] = "text/html";
    mime["css"] = "text/css";
    mime["gif"] = "image/gif";
    mime["jpeg"] = "image/jpeg";
    mime["jpg"] = "image/jpeg";
    mime["js"] = "application/javascript";
    mime["atom"] = "application/atom+xml";
    mime["rss"] = "application/rss+xml";
    mime["mml"] = "text/mathml";
    mime["txt"] = "text/plain";
    mime["jad"] = "text/vnd.sun.j2me.app-descriptor";
    mime["wml"] = "text/vnd.wap.wml";
    mime["htc"] = "text/x-component";
}