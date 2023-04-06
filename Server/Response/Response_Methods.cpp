#include "Response.hpp"
#include <thread>

int	Response::handel_delete_request(std::string file_path)
{

    std::string res;
    std::string file_content;
    int ret = 0;

	if (CheckIsFile(file_path.substr(1)))
	{
        if (access(file_path.substr(1).data(), W_OK) == 0) 
        {
            if (remove(file_path.substr(1).c_str()) == 0)
                res = "HTTP/1.1 204 No Content\r\nContent-type: text/html\r\n" + this->server_cookies + "\r\n";
        } 
        else 
        {
            res = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n" + this->server_cookies + "\r\n";
            file_content = read_file_content(this->error_pages["403"]);
            res += file_content;
        }
	}
	else
    {
        res = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n" + this->server_cookies + "\r\n";
        file_content = read_file_content(this->error_pages["404"]);
        res += file_content;
    }
    ret = send(this->socket_fd, res.data(), res.length(), 0);
    return (ret);
}


int Response::handle_get_request(std::string Path, std::string contentType)
{
    std::ifstream file(Path.substr(1), std::ios::binary);
    std::string file_content;
    std::string res;
    int ret = 0;


    if (file)
    {
        if (Path.find(".", 0) == std::string::npos)
        {
            ret = serve_index(Path, contentType);
            return (ret);
        }
        else
        {
            ret = serve_other_files(Path, contentType);   
            return (ret);
        }
    }
    else if (Path == "/")
    {
        ret = serve_root_path(Path, contentType);
        return (ret);
    }
    else
    {
        res = check_request_path(Path) + "text/html" + "\r\n" + this->server_cookies + "\r\n";
        file_content = read_file_content(this->error_pages["404"]);
        res += file_content;
        ret = send(this->socket_fd, res.data(), res.length(), 0);
    }
    return (ret);
}