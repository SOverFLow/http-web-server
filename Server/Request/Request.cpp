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
    SetBody(req);
    InitMime(this->mime);
    SetMethod(req);
    SetPath(req);
    this->StatusCode = Is_Request_Well_Formed(req);
    SetContent();
    if (GetMime(this->Path) == "php")
        this->is_Cgi = true;
    else
        this->is_Cgi = false;
}

void Request::SetBody(std::string req)
{
    size_t BodyPos;
    BodyPos = req.find("\r\n\r\n", 0);
    this->Body = req.substr(BodyPos+4);
}

bool Check_Is_Chunked(std::string req)
{
    std::string transfer_encoding_header = "Transfer-Encoding: ";
    size_t transfer_encoding_pos = req.find(transfer_encoding_header);
    if (transfer_encoding_pos == std::string::npos)
        return true;

    size_t transfer_encoding_value_pos = transfer_encoding_pos + transfer_encoding_header.size();
    size_t transfer_encoding_value_end = req.find("\r\n", transfer_encoding_value_pos);
    std::string transfer_encoding_value = req.substr(transfer_encoding_value_pos, transfer_encoding_value_end - transfer_encoding_value_pos);

    return transfer_encoding_value == "chunked";
}

bool Check_Encoding_and_ContentLength(std::string req, std::string method)
{
    int i = 0;
    std::string transfer_encoding_header = "Transfer-Encoding: ";
    size_t transfer_encoding_pos = req.find(transfer_encoding_header);
    std::string ContentLength_header = "Content-Length: ";
    size_t ContentLength_pos = req.find(ContentLength_header);
    if (transfer_encoding_pos == std::string::npos)
        i = 1;
    if (ContentLength_pos == std::string::npos && i && method == "POST")
        return (false);
    return (true);
}


bool Check_Is_Uri_Allowed_Caracters(std::string uri)
{
    std::string allowed_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

    for (size_t i = 0; i < uri.size(); i++) {
        if (allowed_chars.find(uri[i]) == std::string::npos)
            return false;
    }
    return true;
}

bool Check_Is_Uri_Large(std::string uri)
{
    return uri.size() > 2048;
}


bool Check_Request_Body_Size(std::string req)
{
    //do later 
    //check if request body size is larger than max body size in config
    return (true);
}

int Request::Is_Request_Well_Formed(std::string req)
{
    if (!Check_Is_Chunked(req))
        return (501);
    if (!Check_Encoding_and_ContentLength(req, this->Method))
        return (400);
    if (!Check_Is_Uri_Allowed_Caracters(this->Path))
        return (400);
    if (Check_Is_Uri_Large(this->Path))
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