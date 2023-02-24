#include "Response.hpp"
#include <sys/stat.h>

int		CheckIsFile(const std::string& path)
{
	struct stat s;
	if (stat(path.c_str(), &s) == 0 )
	{
		if (s.st_mode & S_IFDIR)
			return 0;
		else if (s.st_mode & S_IFREG)
			return 1;
		else
			return 0;
	}
	else
		return 0;
}


std::string	Response::handel_delete_request(std::string file_path)
{

    std::string res;
    std::string file_content;

	if (CheckIsFile(file_path.substr(1)))
	{
		if (remove(file_path.substr(1).c_str()) == 0)
        {
            res = "HTTP/1.1 204 No Content\r\nContent-type: text/html\r\n\r\n";

        }
		else
        {
            res = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n\r\n";
            file_content = read_file_content("/Error_Pages/403.html");
            res += file_content;
        }
	}
	else
    {
        res = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
        file_content = read_file_content("/Error_Pages/404.html");
        res += file_content;
    }
    return (res);
}

std::string get_index_file_name(std::vector<std::string> index)
{
    for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it) 
    {
        std::cout << "--> " << *it << std::endl;
         //std::ifstream file(*it, std::ios::binary);
    }
    return ("");
}


void Response::serve_index(std::string Path, std::string contentType)
{
    //std::cout << "yes => " << Path.substr(1) << std::endl;
    std::string file_content;
    if (Path.find("/", 1) == std::string::npos)
        Path = Path + "/";
            
    std::string s = Path + "index.html";
    std::ifstream index(s.substr(1), std::ios::binary);
    if (index)
    {
        this->response = check_request_path(s) + contentType + "\r\n\r\n";
        send(this->socket_fd, this->response.data(), this->response.length(), 0);
        file_content = read_file_content(s);
        send(this->socket_fd, file_content.data(), file_content.length(), 0);
    }
}


void Response::serve_other_files(std::string Path, std::string contentType)
{
    std::string file_content;

    //std::cout << "others => " << Path.substr(1) << std::endl;
    this->response = check_request_path(Path) + contentType + "\r\n\r\n";
    send(this->socket_fd, this->response.data(), this->response.length(), 0);
    file_content = read_file_content(Path);
    send(this->socket_fd, file_content.data(), file_content.length(), 0);
}


std::string Response::handle_get_request(std::string Path, std::string contentType)
{
    std::ifstream file(Path.substr(1), std::ios::binary);
    std::string file_content;
    std::string res;


    if (file)
    {
        if (Path.find(".", 0) == std::string::npos)
        {
            serve_index(Path, contentType);
            return (res);
        }
        else
        {
            serve_other_files(Path, contentType);   
            return (res);
        }
    }
    else if (Path == "/")
    {
        //std::cout << "wi dkhalt" << std::endl;

        res = check_request_path("/index.html") + contentType + "\r\n\r\n";
        if (this->Status == 404)
            file_content = read_file_content("/Error_Pages/404.html");
        else
            file_content = read_file_content("/index.html");
            
        res += file_content; 
    }
    else
    {
        res = check_request_path(Path) + "text/html" + "\r\n\r\n";
        file_content = read_file_content("/Error_Pages/404.html");
        res += file_content;
    }
    return (res);
}