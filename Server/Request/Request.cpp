#include "Request.hpp"

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

Request::Request(std::string req)
{
    this->StatusCode = Is_Request_Well_Formed(req);
    InitMime(this->mime);
    SetMethod(req);
    SetPath(req);
    SetContent();
    if (GetMime(this->Path) == "php")
        this->is_Cgi = true;
    else
        this->is_Cgi = false;
}



bool Check_Is_Chunked(std::string req)
{
    return (true);
}

bool Check_Encoding_and_ContentLength(std::string req)
{
    return  (true);
}


bool Check_Is_Uri_Allowed_Caracters(std::string req)
{
    return (true);
}

bool Check_Is_Uri_Large(std::string req)
{
    return (true);
}


bool Check_Request_Body_Size(std::string req)
{
    return (true);
}

int Request::Is_Request_Well_Formed(std::string req)
{
    if (!Check_Is_Chunked(req))
        return (501);
    if (!Check_Encoding_and_ContentLength(req))
        return (400);
    if (!Check_Is_Uri_Allowed_Caracters(req))
        return (400);
    if (!Check_Is_Uri_Large(req))
        return (414);
    if (!Check_Request_Body_Size(req))
        return (413);

    return (200);
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
        if (req[i] == ' ' || req[i] == '?')
            break;
        len++;
    }
    this->Path = req.substr(start, len);
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
    mime["png"] = "image/png";
    mime["js"] = "application/javascript";
    mime["atom"] = "application/atom+xml";
    mime["rss"] = "application/rss+xml";
    mime["mml"] = "text/mathml";
    mime["txt"] = "text/plain";
    mime["jad"] = "text/vnd.sun.j2me.app-descriptor";
    mime["wml"] = "text/vnd.wap.wml";
    mime["htc"] = "text/x-component";
    mime["mp4"] = "video/mp4";
    mime["ico"] = "image/x-icon";
    mime["svg"] = "image/svg+xml";
}