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

std::string chunked_Body(std::string &Body)
{
    std::string New_Body;
    int chunk;
    int pos;
    chunk = atoi(Body.c_str());
    pos = std::to_string(chunk).length() + 2;
    while (chunk)
    {
        New_Body += Body.substr(pos, chunk);
        chunk = atoi(Body.c_str() + pos);
        pos = std::to_string(chunk).length() + 2;
    }
    return (New_Body);
}

Request::Request(std::string req, size_t server_body_size)
{
    
    //std::cout << req << std::endl;
    this->Body_Size_From_Config = server_body_size;
    InitMime(this->mime);
    SetMethod(req);
    SetBody(req);
    SetPath(req);
    this->StatusCode = Is_Request_Well_Formed(req);
    SetContent();
    SetHost(req);
    if (GetMime(this->Path) == "php" || GetMime(this->Path) == "cgi")
        this->is_Cgi = true;
    else
        this->is_Cgi = false;
    if (req.find("Transfer-Encoding: chunked", 0))
        this->Body = chunked_Body(this->Body);
}


void Request::SetBody(std::string req)
{
    if (this->Method == "POST")
    {
        std::size_t content_length_start = req.find("Content-Length: ") + 16;
        std::size_t content_length_end = req.find("\r\n", content_length_start);
        std::string content_length_str = req.substr(content_length_start, content_length_end - content_length_start);
        
        int content_length = std::stoi(content_length_str);
        this->Content_Lenght = content_length;
        size_t BodyPos;
        BodyPos = req.find("\r\n\r\n", 0);
        this->Body = req.substr(BodyPos+4);
    }
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


bool Check_Request_Body_Size(size_t body_size, size_t content_length)
{
    return content_length > body_size;
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
    if (Check_Request_Body_Size(this->Body_Size_From_Config, this->Body.size()))
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
    int Q_start = 0;
    int len = 0;
    int end = 0;
    for(int i = start; req[i]; i++)
    {
        if (req[i] == ' ' || req[i] == '?')
        {
            if (req[i] == '?')
                Q_start = i;
            break;
        }
        len++;
    }
    this->Path = req.substr(start, len);
    if(Q_start > 0)
    {
        for(int i = Q_start; req[i] != ' '; i++)
            end = i;
        this->Qurey_String = req.substr(Q_start, end - Q_start + 1);
    }
    else
        this->Qurey_String = "NULL";
}
void Request::SetHost(std::string req)
{
    int Host_p = 6;
    int end = 0;
    Host_p += req.find("Host: ", 0);
    for(int i = Host_p+6; req[i] != '\n'; i++)
        end = i;
    this->Host = req.substr(Host_p, end - Host_p + 1);
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