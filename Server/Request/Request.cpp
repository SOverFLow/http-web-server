#include "Request.hpp"

Request::Request(std::string req)
{
    InitMime(this->mime);
    SetMethod(req);
    SetPath(req);
    SetContent();
}

Request::~Request(){}

void Request::SetMethod(std::string req)
{
    this->Method = "No";
    if (req.find("GET", 0) != std::string::npos)
        this->Method = "GET";
    else if (req.find("POST", 0) != std::string::npos)
        this->Method = "POST";
    else if (req.find("DELETE", 0) != std::string::npos)
        this->Method = "DELETE";
}

void Request::SetPath(std::string req)
{
    int start = this->Method.length() + 1;
    int len = 0;
    for(int i = start; req[i]; i++)
    {
        if (req[i] == ' ')
            break;
        len++;
    }
    this->Path = req.substr(start, len);
}

std::string GetMime(std::string Path)
{
    int index = 0;
    std::string type;
    for (int i = Path.length() - 1; i != 0; i--)
    {
        if (Path[i] == '.')
        {
            index = i;
            break;
        }
    }
    type = Path.substr(index+1, Path.length() - index);
    return (type);
}

void Request::SetContent()
{
    std::map<std::string, std::string>::iterator it;
    std::string tmp = GetMime(this->Path);
    char *type = &tmp[0];
    it = this->mime.begin();
    while (it != this->mime.end())
    {
        if (strcmp(type, &it->first[0]) == 0)
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