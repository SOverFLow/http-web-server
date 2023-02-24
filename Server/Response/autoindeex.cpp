void Server::respond_to_clients(int client_socket, std::string root_path)
{
    char buffer[1024];
    std::string full_path;
    int num_bytes = recv(client_socket, buffer, sizeof(buffer), 0);
    if (num_bytes == -1) 
    {
        std::cout<< "socket => " << client_socket << std::endl;
        perror("Error receiving data from client");
        close(client_socket);
        return;
    } 
    else if (num_bytes == 0) 
    {
        close(client_socket);
        return;
    }
    Request req(buffer);
    full_path = root_path + req.Path.substr(1);

    // Check if the request path corresponds to a directory
    struct stat path_stat;
    if (stat(full_path.c_str(), &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
    {
        if (req.Path.back() != '/')
        {
            // If the path doesn't end in a trailing slash, redirect to the same path with a trailing slash
            std::string redirect_path = req.Path + "/";
            std::string response = "HTTP/1.1 301 Moved Permanently\r\nLocation: " + redirect_path + "\r\n\r\n";
            send(client_socket, response.c_str(), response.length(), 0);
            close(client_socket);
            return;
        }
        else
        {
            // Generate an HTML document listing the files and subdirectories in the directory
            std::stringstream response;
            response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            response << "<html>\n<head>\n<title>Index of " << req.Path << "</title>\n</head>\n<body>\n<h1>Index of " << req.Path << "</h1>\n<hr />\n<ul>\n";
            DIR *dir;
            struct dirent *ent;
            if ((dir = opendir(full_path.c_str())) != NULL)
            {
                while ((ent = readdir(dir)) != NULL)
                {
                    std::string filename = ent->d_name;
                    if (filename != "." && filename != "..")
                    {
                        std::string path = req.Path + filename;
                        if (S_ISDIR(ent->d_type))
                        {
                            response << "<li><a href=\"" << path << "/\">" << filename << "/</a></li>\n";
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
            close(client_socket);
            return;
        }
    }
       
    if (!req.is_Cgi)
    {
      Response res(full_path, req.Method, req.Content_Type, client_socket, req.is_Cgi);
      this->data = res.res_to_client;
    }
    else
        this->data = Cgi_Handler(full_path, req.Body);
    int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
   
