void Server::respond_to_clients(int client_socket, std::string root_path)
{
    // Set a limit on the maximum allowed size of the request body
    const int MAX_BODY_SIZE = 1024;

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

    // Check if the request method is POST
    if (req.Method == "POST") {
        // Get the size of the request body
        int content_length = req.Headers["Content-Length"];
        if (content_length > MAX_BODY_SIZE) {
            // If the request body size exceeds the limit, send a response with a 413 status code
            std::string error_message = "Request Entity Too Large";
            std::string response_body = "<html><body><h1>413 Request Entity Too Large</h1></body></html>";
            std::string response_headers = "HTTP/1.1 413 Request Entity Too Large\r\n"
                                           "Content-Type: text/html\r\n"
                                           "Content-Length: " + std::to_string(response_body.length()) + "\r\n"
                                           "\r\n";
            std::string response = response_headers + response_body;
            send(client_socket, response.c_str(), response.length(), 0);
            close(client_socket);
            return;
        }
    }

    full_path = root_path + req.Path.substr(1);
       
    if (!req.is_Cgi)
    {
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


























void Server::respond_to_clients(int client_socket, std::string root_path, size_t max_body_size)
{
    char buffer[1024];
    std::string full_path;
    size_t num_bytes_received = 0;
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

    num_bytes_received += num_bytes;
    if (num_bytes_received > max_body_size) {
        std::cout << "Client request body size exceeds limit." << std::endl;
        close(client_socket);
        return;
    }

    Request req(buffer);
    full_path = root_path + req.Path.substr(1);
       
    if (!req.is_Cgi)
    {
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
