#include "Response.hpp"

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


std::string get_index_file_name(std::vector<std::string> index, std::string path)
{
    for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it) 
    {
         std::ifstream file(path.substr(1) + *it, std::ios::binary);
         if (file)
            return (path.substr(1) + *it);
    }
    return ("no");
}


void Response::serve_index(std::string Path, std::string contentType)
{
    std::string correct_index;
    correct_index = get_index_file_name(this->index_files, Path);
    std::string file_content;
    if (Path.find("/", 1) == std::string::npos)
        Path = Path + "/";
            
    std::string s = correct_index;
    std::ifstream index(s, std::ios::binary);
    if (index)
    {
        this->response = check_request_path("/"+s) + contentType + "\r\n\r\n";
        send(this->socket_fd, this->response.data(), this->response.length(), 0);
        file_content = read_file_content("/"+s);
        send(this->socket_fd, file_content.data(), file_content.length(), 0);
    }
    else
    {
        this->response = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n\r\n";
        send(this->socket_fd, this->response.data(), this->response.length(), 0);
        file_content = read_file_content("/Error_Pages/403.html");
        send(this->socket_fd, file_content.data(), file_content.length(), 0);
    }
}


void Response::serve_other_files(std::string Path, std::string contentType)
{
    std::string file_content;

    this->response = check_request_path(Path) + contentType + "\r\n\r\n";
    send(this->socket_fd, this->response.data(), this->response.length(), 0);
    file_content = read_file_content(Path);
    send(this->socket_fd, file_content.data(), file_content.length(), 0);
}


void Response::serve_root_path(std::string Path, std::string contentType)
{
    std::string res;
    std::string file_content;
    std::string correct_index;

    correct_index = get_index_file_name(this->index_files, Path);

    std::ifstream index(correct_index, std::ios::binary);
    if (index)
    {
        this->response = check_request_path("/"+correct_index) + contentType + "\r\n\r\n";
        send(this->socket_fd, this->response.data(), this->response.length(), 0);
        file_content = read_file_content("/"+correct_index);
        send(this->socket_fd, file_content.data(), file_content.length(), 0);
    }
    else
    {
        this->response = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n\r\n";
        send(this->socket_fd, this->response.data(), this->response.length(), 0);
        file_content = read_file_content("/Error_Pages/403.html");
        send(this->socket_fd, file_content.data(), file_content.length(), 0);
    }
}