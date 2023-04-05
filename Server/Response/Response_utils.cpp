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
        file_content = read_file_content("/"+s);
        this->response += file_content;
        send(this->socket_fd, this->response.data(), this->response.length(), 0);
        return ;
    }
    else
    {
        if (!auto_index)
        {
            this->response = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n" + this->server_cookies + "\r\n";
            file_content = read_file_content(this->error_pages["403"]);
            this->response += file_content;
            send(this->socket_fd, this->response.data(), this->response.length(), 0);
        }
        else
            serve_auto_index(this->full_path, this->req_path, this->socket_fd);
    }
}



void send_data_v_2(int sockfd, const char* data, size_t size, size_t chunk_size) 
{
    if (size == 0)
        return;
    
    size_t send_size = std::min(chunk_size, size);

    // std::cout << "Sending " << data << std::endl;
    int sent = send(sockfd, data, send_size, 0);
    
    // if (sent == -1) {
    //     return ;
    // }

    send_data_v_2(sockfd, data + sent, size - sent, chunk_size);
}


void send_data(int sockfd, const char* data, size_t data_size)
{

    const char* data_ptr = data;
    int remaining = data_size;
    int chunk_size = 1024;

    while (remaining > 0) 
    {
        int send_size = std::min(chunk_size, remaining);

        int sent = send(sockfd, data_ptr, send_size, 0);

        remaining -= sent;
        data_ptr += sent;
    }

}

void Response::serve_other_files(std::string Path, std::string contentType)
{
    std::string file_content;
    std::cout << "other files" << std::endl;

    try
    {
        this->response = check_request_path(Path) + contentType + "\r\n" + this->server_cookies + "\r\n";
        send(this->socket_fd, this->response.data(), this->response.length(), 0);
        file_content = read_file_content(Path);
        send_data(this->socket_fd, file_content.data(), file_content.length());
    }
    catch(const std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
   
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
       
        if (is_header_send == false)
        {
            this->response = check_request_path("/"+correct_index) + contentType + "\r\n" + this->server_cookies + "\r\n";
            file_content = read_file_content("/"+correct_index);
            this->response += file_content;
            send(this->socket_fd, this->response.data(), this->response.length(), 0);
            is_header_send = true;
            return ;
        }
    }
    else
    {
        if (!auto_index)
        {
            this->response = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n" + this->server_cookies + "\r\n";
            file_content = read_file_content(this->error_pages["403"]);
            this->response += file_content;
            send(this->socket_fd, this->response.data(), this->response.length(), 0);
            return ;
            
        }
        else
            serve_auto_index(this->full_path, this->req_path, this->socket_fd);
    }
}



void serve_auto_index(std::string full_path, std::string req_path, int client_socket)
{
    struct stat path_stat;
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    std::string all = cwd + full_path;
    if (stat( all.c_str(), &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
    {
        std::stringstream response;
        response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        response << "<html>\n<head>\n<title>Index of " << req_path << "</title>\n</head>\n<body>\n<h1>Index of " << req_path << "</h1>\n<hr />\n<ul>\n";
        DIR *dir;
        struct dirent *ent;

        if ((dir = opendir(all.c_str())) != NULL)
            {
            if (full_path != "/")
                full_path.append("/");
            while ((ent = readdir(dir)) != NULL)
            {
                std::string filename = ent->d_name;
                if (filename != "." && filename != "..")
                {
                    std::string path = filename;
                    
                    if (S_ISDIR(ent->d_type))
                    {
                        response << "<li><a href=\"" << full_path + filename << "/\">" << filename << "/</a></li>\n";
                    }
                    else
                    {

                        response << "<li><a href=\"" <<  full_path + filename << "\">" << filename << "/</a></li>\n";
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