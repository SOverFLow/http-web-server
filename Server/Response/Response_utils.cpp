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
        this->response = check_request_path("/"+s) + contentType + "\r\n" + this->server_cookies + "\r\n";
        send(this->socket_fd, this->response.data(), this->response.length(), 0);
        file_content = read_file_content("/"+s);
        send(this->socket_fd, file_content.data(), file_content.length(), 0);
    }
    else
    {
        if (!auto_index)
        {
            this->response = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n" + this->server_cookies + "\r\n";
            send(this->socket_fd, this->response.data(), this->response.length(), 0);
            file_content = read_file_content("/Error_Pages/403.html");
            send(this->socket_fd, file_content.data(), file_content.length(), 0);
        }
        else
            serve_auto_index(this->full_path, this->req_path, this->socket_fd);
    }
}


void Response::serve_other_files(std::string Path, std::string contentType)
{
    std::string file_content;

    this->response = check_request_path(Path) + contentType + "\r\n" + this->server_cookies + "\r\n";
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
        this->response = check_request_path("/"+correct_index) + contentType + "\r\n" + this->server_cookies + "\r\n";
        send(this->socket_fd, this->response.data(), this->response.length(), 0); 
        file_content = read_file_content("/"+correct_index);
        send(this->socket_fd, file_content.data(), file_content.length(), 0);
    }
    else
    {
        if (!auto_index)
        {
            this->response = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n" + this->server_cookies + "\r\n";
            send(this->socket_fd, this->response.data(), this->response.length(), 0);
            file_content = read_file_content("/Error_Pages/403.html");
            send(this->socket_fd, file_content.data(), file_content.length(), 0);
        }
        else
            serve_auto_index(this->full_path, this->req_path, this->socket_fd);
    }
}


void moved_permanetly(std::string path, int client_socket)
{
    if (path.back() != '/')
    {
        std::string redirect_path = path + "/";
        std::string response = "HTTP/1.1 301 Moved Permanently\r\nLocation: " + redirect_path + "\r\n\r\n";
        send(client_socket, response.c_str(), response.length(), 0);
    }
}


void serve_auto_index(std::string full_path, std::string req_path, int client_socket)
{
    struct stat path_stat;
    std::string all = "/Users/selhanda/Desktop/webserv" + full_path;
    if (stat( all.c_str(), &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
    {
        std::stringstream response;
        response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        response << "<html>\n<head>\n<title>Index of " << req_path << "</title>\n</head>\n<body>\n<h1>Index of " << req_path << "</h1>\n<hr />\n<ul>\n";
        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir(all.c_str())) != NULL)
            {
            while ((ent = readdir(dir)) != NULL)
            {
                std::string filename = ent->d_name;
                if (filename != "." && filename != "..")
                {
                    std::string path = filename;
                    
                    if (S_ISDIR(ent->d_type))
                    {
                        response << "<li><a href=\"" <<  path << "/\">" << filename << "/</a></li>\n";
                    }
                    else
                    {

                        response << "<li><a href=\"" << path << "\">" << filename << "</a></li>\n";
                    }
                }
            }
            closedir(dir);
        }
        response << "</ul>\n<hr />\n</body>\n</html>\n";
        std::string response_str = response.str();
        send(client_socket, response_str.c_str(), response_str.length(), 0);
        return;
}
}