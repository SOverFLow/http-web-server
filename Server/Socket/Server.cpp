#include "Server.hpp"

Server::Server(Config config)
{

    path_check = 0;
    client_first_read = false;
    end_of_file = false;
    check_upload_status = 0;
    this->file_bytes_received = 0;
    remove_client = false;
    connection(config.Servers);
}


void Server::connection(std::vector<ServerBlock> &servers)
{
    this->sockets = setup_sockets(servers);
    this->pollfds = create_pollfds(servers);

    int tmp = 0;
    while(1)
    {
        int ready_count = poll(&pollfds[0], pollfds.size(), 1000);
        if (ready_count == -1) {
            std::cout << "Error in poll" << std::endl;
            exit(1);
        }
        
        for (size_t i = 0; i < pollfds.size() && ready_count > 0 ; i++) {
    
            if (pollfds[i].revents & POLLIN) 
            {
                if (pollfds[i].fd == servers[i].sock_fd) 
                {
                    handle_new_connection(servers[i].sock_fd, pollfds);
                    tmp = i;
                }
                else
                {
                    try
                    {
                        respond_to_clients(pollfds[i].fd, root_paths[tmp], servers[tmp], tmp, i);
                        if (remove_client == true)
                        {
                            this->pollfds[i].fd = -1;
                            remove_client = false;
                        }
                        
                    }
                    catch(const std::exception& e)
                    {
                        close(pollfds[i].fd);
                        continue;
                    }
                }
                    
            }
        }

        for (std::vector<pollfd>::iterator it = pollfds.begin(); it != pollfds.end(); ++it) {
            if (it->fd == -1) 
            {
                pollfds.erase(it);
                --it;
            }
        }
    }
}



std::vector<int> Server::setup_sockets(std::vector<ServerBlock> &servers)
{
    std::vector<int> ret_sockets;


    std::cout << "==============================\n";
    std::cout << "======= webserv running ======\n";
    std::cout << "==============================\n";
    for (size_t i =0; i < servers.size(); i++)
    {
        struct sockaddr_in address;
        int option = 1;
        servers[i].sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (servers[i].sock_fd == 0)
        {
            std::cout << "error in the socket" << std::endl;
            exit(1);
        }
        fcntl(servers[i].sock_fd, F_SETFL, O_NONBLOCK);
        setsockopt(servers[i].sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        setsockopt(servers[i].sock_fd, SOL_SOCKET, SO_NOSIGPIPE, &option, sizeof(option));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(servers[i].port);

        if (bind(servers[i].sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            std::cout << "error in binding" << std::endl;
            exit(1);
        }


        if (listen(servers[i].sock_fd, 10000) < 0)
        {
            std::cout << "error in listening" << std::endl;
            exit(1);  
        }
        ret_sockets.push_back(servers[i].sock_fd);
        root_paths.push_back(servers[i].root);
        std::cout << "listening on port => " << servers[i].port << std::endl;

    }
    return (ret_sockets);
}


void Server::respond_to_clients(int client_socket, std::string root_path, ServerBlock server, int tmp, int i)
{
    std::string request_message;
    int bytes_received;
    char buffer[1024];
    std::string full_path;
    bool alreadysent = false;
    int num_sent  = 0;

    buffer[0] = '\0';
    bytes_received = recv(client_socket, buffer, 1024, 0);
    if (bytes_received <= 0)
    {
        this->remove_client = true;
        return ;
    }
    if (bytes_received != -1)
    {
        request_message = std::string(buffer,bytes_received);
        
        if (client_first_read == false)
        {
            Request req(request_message, server.client_max_body_size);
            this->req = req;
            if (req.Method == "No")
            {
                this->data = "HTTP/1.1 405 Method Not Allowed\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                this->data += Return_File_Content(server.error_pages["405"]);
                num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                if (num_sent <= 0) 
                {
                    this->remove_client = true;
                    close(client_socket);
                    return;
                }
                alreadysent = true;
                close(client_socket);
                return ;
            }
            else if (req.Method == "DELETE")
            {
                std::string str = req.Path;
                std::string filename;
                std::string root_plus_file;
                std::string root_location;
                std::size_t found = str.find_last_of("/");
                if (found != std::string::npos) {
                    filename = str.substr(found + 1);
                    str = str.substr(0, found);
                }
                if (check_if_url_is_location(str.substr(1), server.Locations))
                {
                    if (Check_is_method_allowed(req.Method, get_location(str.substr(1), server.Locations).allowed_method))
                    {
                        root_location = get_root_location(str.substr(1), server.Locations);
                        root_plus_file = root_location + filename;
                       
                        Response res(root_plus_file, "DELETE", req.Content_Type,
                                client_socket, req.is_Cgi, tmp_index, get_location(str.substr(1), server.Locations).autoindex, root_plus_file, root_plus_file, true, cookies_part, server.error_pages);
                        num_sent = res.num_sent;
                        alreadysent = true;
                        if (num_sent <= 0) 
                        {
                            this->remove_client = true;
                            close(client_socket);
                            return;
                        }
                        close(client_socket);
                        return ;
                    }
                    else
                    {
                        this->data = "HTTP/1.1 405 Method Not Allowed\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                        this->data += Return_File_Content(server.error_pages["405"]);
                        num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                        if (num_sent <= 0) 
                        {
                            this->remove_client = true;
                            close(client_socket);
                            return;
                        }
                        alreadysent = true;
                        close(client_socket);
                        return ;
                    }
                }
            }
        }
    
        this->error_pages = server.error_pages;
        this->cookies = parse_cookies(request_message);
        this->cookies_part = manage_cookies_session_server();
        
        
        size_t size_of_server_names = server.server_name.size();
        bool server_name_check = false;
        if (size_of_server_names > 0)
        {
            std::vector<std::string>::iterator server_name_iter = server.server_name.begin();
            size_t num_pos = req.Host.find(":");
            std::string host;
            if (num_pos != std::string::npos)
                host = req.Host.substr(0, num_pos);
            else
                host = req.Host;
            while (server_name_iter != server.server_name.end())
            {
                if (host == *server_name_iter)
                {
                    server_name_check = true;
                    break ;
                }
                if (host[0] == '1' || host[0] == '0')
                {
                    server_name_check = true;
                    break ;
                }
                server_name_iter++;
            }
            if (server_name_check == false)
            {
                this->data = "HTTP/1.1 503 Service Unavailable\r\nContent-type: text/html\r\n" + this->cookies_part + "\r\n";
                this->data += Return_File_Content(server.error_pages["503"]);
                num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                close(client_socket);
                return ;
                if (num_sent <= 0) 
                {
                    this->remove_client = true;
                    close(client_socket);
                    return;
                }
            }
        }
        

    if (req.StatusCode != 200)
    {
        this->data = Return_Error_For_Bad_Request(req.StatusCode);
        num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
        close(client_socket);
        return ;
        if (num_sent <= 0) 
        {
            this->remove_client = true;
            close(client_socket);
            return;
        }
    }

    else 
    {
        if (req.is_Cgi)
        {
            std::string str = req.Path;
            std::string filename;
            std::string lang;
            std::string root_plus_file;
            std::size_t found = str.find_last_of("/");
            if (found != std::string::npos) {
                filename = str.substr(found + 1);
                str = str.substr(0, found);
            }

            if (str.empty())
            {
                size_t lang_pos = filename.find(".");
                lang = filename.substr(lang_pos + 1);
                if (lang == "pl" || lang == "cgi")
                    lang = ".cgi";
                root_plus_file = server.root + req.Path.substr(1);
                this->data = Cgi_Handler(req, root_plus_file, NULL, lang, server, this->cookies_part);
                    if (req.cgiStatus == 404)
                        this->data += Return_File_Content(server.error_pages["404"]);
                    else if (req.cgiStatus == 403)
                        this->data += Return_File_Content(server.error_pages["403"]);
                    else if (req.cgiStatus == 500)
                        this->data += Return_File_Content(server.error_pages["500"]);
                num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                close(client_socket);
                if (num_sent <= 0) 
                {
                    this->remove_client = true;
                    close(client_socket);
                    return;
                }
                return ;
            }
           
            else if (check_if_url_is_location(str.substr(1), server.Locations))
            { 
                if (check_if_location_has_redirect(str.substr(1), server.Locations))
                {
                    int code = get_redirect_code_for_location(str.substr(1), server.Locations);
                    std::string msg = return_redirect_msg(code);
                    
                    this->data = "HTTP/1.1 " + std::to_string(code) + " " + msg + "\r\nLocation: " + get_redirect_url_for_location(str.substr(1), server.Locations) + "\r\n\r\n";
                    num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                    if (num_sent <= 0) 
                    {
                        this->remove_client = true;
                        close(client_socket);
                        return;
                    }
                    close(client_socket);
                    return ;
                }

                std::string root_path = get_root_location(str.substr(1), server.Locations);
                std::string all_path = root_path + filename;
                if (req.Method == "POST")
                {
                        if (Check_upload_Location_Status(str.substr(1), server.Locations))
                        {
                            std::string dir_path = get_root_location(str.substr(1), server.Locations) + Get_upload_Location_Path(str.substr(1),server.Locations);
                            if(client_first_read == false)
                            {
                                check_upload_status = parse_upload_post_data(request_message, dir_path);
                                file_content_length = req.Content_Lenght;
                                if (file_content_length == 0)
                                {
                                    this->data = Cgi_Handler(req, all_path, NULL, get_location(req.Path.substr(1), server.Locations).CgiLang, server, this->cookies_part);
                                    num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                                    if (num_sent <= 0) 
                                    {
                                        this->remove_client = true;
                                        close(client_socket);
                                        return;
                                    }
                                    close(client_socket);
                                    return ;
                                }
                                client_first_read = true;
                                file_bytes_received = this->first_read_data_size;
                                return ;
                            }
                            else if (check_upload_status && client_first_read == true)
                            {
                                    file_bytes_received += bytes_received;
                                    check_upload_status = parse_upload_post_data_part_two(request_message, dir_path);
                                    if (check_upload_status)
                                    {
                                        std::cout << "upload >>"<< file_bytes_received << std::endl;
                            
                                        if (end_of_file)
                                        {
                                            client_first_read = false;
                                            remove_client =  true;
                                            this->data = "HTTP/1.1 201 Created\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                                            this->data += Return_File_Content("/Error_Pages/201.html");
                                            end_of_file = false;
                                        }
                                        else
                                            return ;
                                    }
                                    else if (check_upload_status == -1)
                                    {
                                        client_first_read = false;
                                        this->data = "HTTP/1.1 500 Internal Server Error\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                                        this->data += Return_File_Content("/Error_Pages/500.html");
                                    }
                            }
                            else
                            {
                                this->data = Cgi_Handler(req, all_path, NULL, get_location(str.substr(1), server.Locations).CgiLang, server, this->cookies_part);
                            }
                        }
                }

                else 
                {
                    std::ifstream file(all_path.substr(1), std::ios::binary);
                    if (file)
                        this->data = Cgi_Handler(req, all_path, NULL, get_location(str.substr(1), server.Locations).CgiLang, server, this->cookies_part);
                    else
                    {
                        this->data = Cgi_Handler(req, all_path, NULL, get_location(str.substr(1), server.Locations).CgiLang, server, this->cookies_part);
                        if (req.cgiStatus == 404)
                            this->data += Return_File_Content(server.error_pages["404"]);
                        else if (req.cgiStatus == 403)
                            this->data += Return_File_Content(server.error_pages["403"]);
                        else if (req.cgiStatus == 500)
                            this->data += Return_File_Content(server.error_pages["500"]);
                    }
                }


                num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                if (num_sent <= 0) 
                {
                    this->remove_client = true;
                    close(client_socket);
                    return;
                }
                close(client_socket);
                return ;
            }
        }

        try {
            full_path = root_path + req.Path.substr(1);
        }
        catch(const std::exception &e)
        {
            std::cout << "error => " << e.what() << std::endl;
        }

        if (check_if_url_is_location(req.Path.substr(1), server.Locations))
        {
            if (check_if_location_has_redirect(req.Path.substr(1), server.Locations))
            {
                int code = get_redirect_code_for_location(req.Path.substr(1), server.Locations);
                std::string msg = return_redirect_msg(code);
                    
                this->data = "HTTP/1.1 " + std::to_string(code) + " " + msg + "\r\nLocation: " + get_redirect_url_for_location(req.Path.substr(1), server.Locations) + "\r\n\r\n";
                num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                if (num_sent <=  0) 
                {
                    this->remove_client = true;
                    close(client_socket);
                    return;
                }
                close(client_socket);
                return ;
            }
            
            try
            {
                full_path = get_root_location(req.Path.substr(1), server.Locations);
                tmp_path = full_path;
                tmp_index = get_index_location(req.Path.substr(1), server.Locations);
                tmp_methods = get_allowed_methods(req.Path.substr(1), server.Locations);
                path_check = tmp;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            
            
        }
        else if (tmp == path_check && req.Path != "/")
        {
            full_path = tmp_path + req.Path;
        }
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
                        num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                        if (num_sent <= 0) 
                        {
                            this->remove_client = true;
                            close(client_socket);
                            return;
                        }
                        close(client_socket);
                        return ;
                    }
                    
                    std::string root_path = get_root_location(req.Path.substr(1), server.Locations);
                    std::string all_path = "/" + serve_index_for_cgi(root_path, get_index_location(req.Path.substr(1), server.Locations));

                    if (req.Method == "POST")
                    {
                        if (Check_upload_Location_Status(req.Path.substr(1), server.Locations))
                        {
                            std::string dir_path = get_root_location(req.Path.substr(1), server.Locations) + Get_upload_Location_Path(req.Path.substr(1),server.Locations);
                            
                            if(client_first_read == false)
                            {
                                check_upload_status = parse_upload_post_data(request_message, dir_path);
                                file_content_length = req.Content_Lenght;
                                if (file_content_length == 0)
                                {
                                    this->data = Cgi_Handler(req, all_path, NULL, get_location(req.Path.substr(1), server.Locations).CgiLang, server, this->cookies_part);
                                    num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                                    if (num_sent <= 0) 
                                    {
                                        this->remove_client = true;
                                        close(client_socket);
                                        return;
                                    }
                                    close(client_socket);
                                    return ;
                                }
                                client_first_read = true;
                                file_bytes_received = this->first_read_data_size;
                                return ;
                            }
                            else if (check_upload_status && client_first_read == true)
                            {
                                    file_bytes_received += bytes_received;
                                    check_upload_status = parse_upload_post_data_part_two(request_message, dir_path);
                                    if (check_upload_status)
                                    {
                                        std::cout << "upload >>"<< file_bytes_received << std::endl;
                            
                                        if (end_of_file)
                                        {
                                            client_first_read = false;
                                            remove_client =  true;
                                            this->data = "HTTP/1.1 201 Created\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                                            this->data += Return_File_Content("/Error_Pages/201.html");
                                            end_of_file = false;
                                        }
                                        else
                                            return ;
                                    }
                                    else if (check_upload_status == -1)
                                    {
                                        client_first_read = false;
                                        this->data = "HTTP/1.1 500 Internal Server Error\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                                        this->data += Return_File_Content("/Error_Pages/500.html");
                                    }
                            }
                            else
                            {
                                this->data = Cgi_Handler(req, all_path, NULL, get_location(req.Path.substr(1), server.Locations).CgiLang, server, this->cookies_part);
                            }
                        }
                    }
                    else
                    {
                        std::ifstream file(all_path.substr(1), std::ios::binary);
                    
                        if (file)
                        {
                            this->data = Cgi_Handler(req, all_path, NULL, get_location(req.Path.substr(1), server.Locations).CgiLang, server, this->cookies_part);
                        }
                        else
                        {
                            this->data = Cgi_Handler(req, all_path, NULL, get_location(req.Path.substr(1), server.Locations).CgiLang, server, this->cookies_part);
                            if (req.cgiStatus == 404)
                                this->data += Return_File_Content(server.error_pages["404"]);
                            else if (req.cgiStatus == 403)
                                this->data += Return_File_Content(server.error_pages["403"]);
                            else if (req.cgiStatus == 500)
                                this->data += Return_File_Content(server.error_pages["500"]);
                        }
                    }
                    }

                else
                {
                    if (req.Method == "POST")
                    {
                        if (Check_upload_Location_Status(req.Path.substr(1), server.Locations))
                        {
                            std::string dir_path = get_root_location(req.Path.substr(1), server.Locations) + Get_upload_Location_Path(req.Path.substr(1),server.Locations);
                           if(client_first_read == false)
                            {
                                check_upload_status = parse_upload_post_data(request_message, dir_path);
                                file_content_length = req.Content_Lenght;
                                if (file_content_length == 0)
                                {
                                    Response res(full_path, "GET", req.Content_Type,
                                    client_socket, req.is_Cgi, tmp_index, get_location(req.Path.substr(1), server.Locations).autoindex, full_path, req.Path, true, cookies_part, server.error_pages);
                                    num_sent = res.num_sent;
                                    if (num_sent <= 0) 
                                    {
                                        this->remove_client = true;
                                        close(client_socket);
                                        return;
                                    }
                                    alreadysent = true;
                                    close(client_socket);
                                    return ;
                                }
                                client_first_read = true;
                                file_bytes_received = this->first_read_data_size;
                                return ;
                            }
                            else if (check_upload_status && client_first_read == true)
                            {
                                    file_bytes_received += bytes_received;
                                    check_upload_status = parse_upload_post_data_part_two(request_message, dir_path);
                                    if (check_upload_status)
                                    {
                                        std::cout << "upload >>"<< file_bytes_received << std::endl;
                            
                                        if (end_of_file)
                                        {
                                            client_first_read = false;
                                            remove_client =  true;
                                            this->data = "HTTP/1.1 201 Created\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                                            this->data += Return_File_Content("/Error_Pages/201.html");
                                            end_of_file = false;
                                        }
                                        else
                                            return ;
                                    }
                                    else if (check_upload_status == -1)
                                    {
                                        client_first_read = false;
                                        this->data = "HTTP/1.1 500 Internal Server Error\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                                        this->data += Return_File_Content("/Error_Pages/500.html");
                                    }
                            }
                            else
                            {
                                Response res(full_path, "GET", req.Content_Type,
                                client_socket, req.is_Cgi, tmp_index, get_location(req.Path.substr(1), server.Locations).autoindex, full_path, req.Path, true, cookies_part, server.error_pages);
                                num_sent = res.num_sent;
                                alreadysent = true;
                            }
                        }
                        else
                        {
                            Response res(full_path, "GET", req.Content_Type,
                            client_socket, req.is_Cgi, tmp_index, get_location(req.Path.substr(1), server.Locations).autoindex, full_path, req.Path, true, cookies_part, server.error_pages);
                            num_sent = res.num_sent;
                            alreadysent = true;
                        }
                    }
                    else 
                    {
                        Response res(full_path, req.Method, req.Content_Type,
                        client_socket, req.is_Cgi, tmp_index, server.autoindex, full_path, req.Path, true, cookies_part, server.error_pages);
                        num_sent = res.num_sent;
                        alreadysent = true;
                    }
                }


            }
            else if (!Check_is_method_allowed(req.Method, server.allowed_method))
            {
                this->data = "HTTP/1.1 405 Method Not Allowed\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                this->data += Return_File_Content(server.error_pages["405"]);
                num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
                if (num_sent <= 0) 
                {
                    this->remove_client = true;
                    close(client_socket);
                    return;
                }
                alreadysent = true;
                close(client_socket);
                return ;
    
            }
            else if (Check_is_method_allowed(req.Method, server.allowed_method) && !check_if_url_is_location(req.Path.substr(1), server.Locations))
            {
                try
                {
                    Response res(full_path, req.Method, req.Content_Type,
                    client_socket, req.is_Cgi, server.index, server.autoindex, full_path, req.Path, false, cookies_part, server.error_pages);
                    num_sent = res.num_sent;
                    alreadysent = true;
                    this->remove_client = true;
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
                
                
            }
            else if (Check_is_method_allowed(req.Method, get_location(req.Path.substr(1), server.Locations).allowed_method) && check_if_url_is_location(req.Path.substr(1), server.Locations))
            {
                Response res(full_path, req.Method, req.Content_Type,
                client_socket, req.is_Cgi, tmp_index, get_location(req.Path.substr(1), server.Locations).autoindex, full_path, req.Path, true, cookies_part, server.error_pages);
                num_sent = res.num_sent;
                alreadysent = true;
            }
            else
            {
                this->data = "HTTP/1.1 405 Method Not Allowed\r\nContent-type: text/html\r\n" + cookies_part + "\r\n";
                this->data += Return_File_Content(server.error_pages["405"]);
            }
            
    }

    if (alreadysent == false)
    {
        num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
    }

    close(this->pollfds[i].fd);
    
    if (num_sent <= 0) 
    {
        this->remove_client = true;
        close(client_socket);
        return;
    }
    }
}

