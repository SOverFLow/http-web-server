#include "Server.hpp"

Server::Server(Config config)
{
    path_check = 0;
    connection(config.Servers);
}


Client::Client()
{

}


void Server::connection(std::vector<ServerBlock> &servers)
{
    this->sockets = setup_sockets(servers);
    std::vector<pollfd> pollfds = create_pollfds(servers);
    std::vector<Client> clients;

    int tmp = 0;
    while(1)
    {
        int ready_count = poll(&pollfds[0], pollfds.size(), -1);
        if (ready_count == -1) {
            std::cout << "Error in poll" << std::endl;
            exit(1);
        }

        for (int i = 0; i < pollfds.size(); i++) {
            if (pollfds[i].revents & POLLIN) {
                if (pollfds[i].fd == servers[i].sock_fd) 
                {
                   
                    handle_new_connection(servers[i].sock_fd, pollfds);
                    tmp = i;
                }
                 else 
                 {
                    respond_to_clients(pollfds[i].fd, root_paths[tmp], servers[tmp], tmp);
                 }
            }
        }
    }
}



std::vector<int> Server::setup_sockets(std::vector<ServerBlock> &servers)
{
    std::vector<int> ret_sockets;

    for (int i =0; i < servers.size(); i++)
    {
        struct sockaddr_in address;
        int option = 1;
        servers[i].sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (servers[i].sock_fd == 0)
        {
            std::cout << "error in the socket" << std::endl;
            exit(1);
        }
        setsockopt(servers[i].sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(servers[i].port);

        if (bind(servers[i].sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            std::cout << "error in binding" << std::endl;
            exit(1);
        }


        if (listen(servers[i].sock_fd, 10) < 0)
        {
            std::cout << "error in listening" << std::endl;
            exit(1);  
        }
        set_nonblocking(servers[i].sock_fd);
        ret_sockets.push_back(servers[i].sock_fd);
        root_paths.push_back(servers[i].root);
        std::cout << "listening on port => " << servers[i].port << std::endl;

    }
    return (ret_sockets);
}


void Server::respond_to_clients(int client_socket, std::string root_path, ServerBlock server, int tmp)
{
    std::string full_request;
    int bytes_received;
    char buffer[8000000];

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        full_request += std::string(buffer, bytes_received);
    }
    std::string full_path;
    //int num_bytes = recv(client_socket, buffer, sizeof(buffer), 0);
    // if (bytes_received == -1) 
    // {
    //     std::cout << "Error receiving data from client" << std::endl;
    //     close(client_socket);
    //     return;
    // } 
    // else if (bytes_received == 0) 
    // {
    //     close(client_socket);
    //     return;
    // }

    Request req(full_request, server.client_max_body_size);


    if (req.StatusCode != 200)
        this->data = Return_Error_For_Bad_Request(req.StatusCode);

    else 
    {
        if (req.is_Cgi)
        {
            std::string str = req.Path;
            std::string filename;
            std::size_t found = str.find_last_of("/");
            if (found != std::string::npos) {
                filename = str.substr(found + 1);
                str = str.substr(0, found);
            }

            if (str.empty())
            {
                std::ifstream file(req.Path.substr(1), std::ios::binary);
                if (file)
                    this->data = Cgi_Handler(req.Path, NULL);
                else
                {
                    this->data = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
                    this->data += Return_File_Content("/Error_Pages/404.html");
                }
                int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                close(client_socket);
                return ;
            }
           
            else if (check_if_url_is_location(str.substr(1), server.Locations))
            { 
                if (check_if_location_has_redirect(str.substr(1), server.Locations))
                {
                    int code = get_redirect_code_for_location(str.substr(1), server.Locations);
                    std::string msg = return_redirect_msg(code);
                    
                    this->data = "HTTP/1.1 " + std::to_string(code) + " " + msg + "\r\nLocation: " + get_redirect_url_for_location(str.substr(1), server.Locations) + "\r\n\r\n";
                    int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                    close(client_socket);
                    return ;
                }

                std::string root_path = get_root_location(str.substr(1), server.Locations);
                std::string all_path = root_path + filename;
                std::ifstream file(all_path.substr(1), std::ios::binary);
                if (file)
                    this->data = Cgi_Handler(all_path, NULL);
                else
                {
                    this->data = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
                    this->data += Return_File_Content("/Error_Pages/404.html");
                }
                int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                close(client_socket);
                return ;
            }
        }

        full_path = root_path + req.Path.substr(1);

        if (check_if_url_is_location(req.Path.substr(1), server.Locations))
        { 
            if (check_if_location_has_redirect(req.Path.substr(1), server.Locations))
            {
                int code = get_redirect_code_for_location(req.Path.substr(1), server.Locations);
                std::string msg = return_redirect_msg(code);
                    
                this->data = "HTTP/1.1 " + std::to_string(code) + " " + msg + "\r\nLocation: " + get_redirect_url_for_location(req.Path.substr(1), server.Locations) + "\r\n\r\n";
                int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                close(client_socket);
                return ;
            }
            
            full_path = get_root_location(req.Path.substr(1), server.Locations);
            tmp_path = full_path;
            tmp_index = get_index_location(req.Path.substr(1), server.Locations);
            tmp_methods = get_allowed_methods(req.Path.substr(1), server.Locations);
            path_check = tmp;
        }
        else if (tmp == path_check && req.Path != "/")
        {
            full_path = tmp_path + req.Path;
        }

        // cookie_handler(buffer);
        
        
            if (tmp == path_check && req.Path != server.root 
            && Check_is_method_allowed(req.Method, tmp_methods))
            {
                if (Check_Cgi_Location_Status(req.Path.substr(1), server.Locations))
                {   
                   if (check_if_location_has_redirect(req.Path.substr(1), server.Locations))
                    {
                        int code = get_redirect_code_for_location(req.Path.substr(1), server.Locations);
                        std::string msg = return_redirect_msg(code);
                    
                        this->data = "HTTP/1.1 " + std::to_string(code) + " " + msg + "\r\nLocation: " + get_redirect_url_for_location(req.Path.substr(1), server.Locations) + "\r\n\r\n";
                        int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                        close(client_socket);
                        return ;
                    }
                    std::string root_path = get_root_location(req.Path.substr(1), server.Locations);
                    std::string all_path = "/" + serve_index_for_cgi(root_path, get_index_location(req.Path.substr(1), server.Locations));

                    std::ifstream file(all_path.substr(1), std::ios::binary);

                    if (file)
                        this->data = Cgi_Handler(all_path, NULL);
                    else
                    {
                        this->data = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n\r\n";
                        this->data += Return_File_Content("/Error_Pages/403.html");
                    }
                }
                else
                {
                    if (req.Method == "POST")
                    {
                        if (Check_upload_Location_Status(req.Path.substr(1), server.Locations))
                        {
                            int check;
                            std::cout << "Uploading... " << std::endl;
                            std::string dir_path = get_root_location(req.Path.substr(1), server.Locations) + Get_upload_Location_Path(req.Path.substr(1),server.Locations);
                            check = parse_upload_post_data(full_request, req.Body, dir_path);
                            if (check)
                            {
                                this->data = "HTTP/1.1 201 Created\r\nContent-type: text/html\r\n\r\n";
                                this->data += Return_File_Content("/Error_Pages/201.html");
                            }
                            else
                            {
                                Response res(full_path, "GET", req.Content_Type,
                                client_socket, req.is_Cgi, tmp_index, server.autoindex, full_path, req.Path, true);
                                this->data = res.res_to_client;
                            }
                        }
                        else
                        {
                            Response res(full_path, "GET", req.Content_Type,
                            client_socket, req.is_Cgi, tmp_index, server.autoindex, full_path, req.Path, true);
                            this->data = res.res_to_client;
                        }
                    }
                    else 
                    {
                        Response res(full_path, req.Method, req.Content_Type,
                        client_socket, req.is_Cgi, tmp_index, server.autoindex, full_path, req.Path, true);
                        this->data = res.res_to_client;
                    }
                }


            }
            else if (Check_is_method_allowed(req.Method, server.allowed_method) && !check_if_url_is_location(req.Path.substr(1), server.Locations))
            {
                Response res(full_path, req.Method, req.Content_Type,
                client_socket, req.is_Cgi, server.index, server.autoindex, full_path, req.Path, false);
                this->data = res.res_to_client;
            }
            else
            {
                this->data = "HTTP/1.1 405 Method Not Allowed\r\nContent-type: text/html\r\n\r\n";
                this->data += Return_File_Content("/Error_Pages/405.html");
            }
    }

    
    int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
    close(client_socket);
    if (num_sent == -1) 
    {
        std::cout << "Error sending data to client";
        close(client_socket);
        return;
    }
}

