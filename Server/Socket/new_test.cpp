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

    // check if the requested path is a directory
    if (full_path.back() == '/')
    {
        full_path += "index.html";
    }
    if (req.Method == "GET" && is_directory(full_path))
    {
        std::string index_path = full_path + "/index.html";
        if (file_exists(index_path))
        {
            // serve the index file
            Response res(index_path, req.Method, req.Content_Type, client_socket, req.is_Cgi);
            this->data = res.res_to_client;
        }
        else
        {
            // list the directory contents
            std::vector<std::string> dir_list = list_directory(full_path);
            std::string dir_html = generate_directory_html(dir_list);
            Response res(dir_html, "200 OK", "text/html", client_socket, req.is_Cgi);
            this->data = res.res_to_client;
        }
    }
    else if (!req.is_Cgi)
    {
        // serve the file
        Response res(full_path, req.Method, req.Content_Type, client_socket, req.is_Cgi);
        this->data = res.res_to_client;
    }
    else
        this->data = Cgi_Handler(full_path, NULL);
    int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
    close(client_socket);
    if (num_sent == -1) 
    {
        perror("Error sending data to client");
        close(client_socket);
        return;
    }
}