#include "Server.hpp"

Server::Server(Config config)
{
    connection(config.Servers);
}

Client::Client()
{

}

void Server::connection(std::vector<ServerBlock> &servers)
{
    fd_set read_fds, write_fds;
    int max_fd = 0;
    std::vector<int> sockets = setup_sockets(servers);
    std::vector<Client> clients(sockets.size());

    while (true)
    {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        max_fd = 0;

        for (int i = 0; i < sockets.size(); i++)
        {
            FD_SET(sockets[i], &read_fds);
            max_fd = std::max(max_fd, sockets[i]);
        }

        for (int i = 0; i < clients.size(); i++)
        {
            if (clients[i].sock >= 0)
            {
                FD_SET(clients[i].sock, &read_fds);
                FD_SET(clients[i].sock, &write_fds);
                max_fd = std::max(max_fd, clients[i].sock);
            }
        }

        int ret = select(max_fd + 1, &read_fds, &write_fds, NULL, NULL);

        if (ret < 0)
        {
            std::cerr << "Error in select: " << strerror(errno) << std::endl;
            break;
        }
        else if (ret == 0)
        {
            // Timeout occurred, continue waiting
            continue;
        }

        for (int i = 0; i < sockets.size(); i++)
        {
            if (FD_ISSET(sockets[i], &read_fds))
            {
                Client new_client;
                new_client.sock = accept(sockets[i], (struct sockaddr *)&new_client.address, (socklen_t*)&new_client.address_len);
                if (new_client.sock < 0)
                {
                    std::cerr << "Error accepting new client: " << strerror(errno) << std::endl;
                    continue;
                }

                // Add the new client to the list
                clients.push_back(new_client);
            }
        }

        for (int i = 0; i < clients.size(); i++)
        {
            if (clients[i].sock >= 0 && (FD_ISSET(clients[i].sock, &read_fds) || FD_ISSET(clients[i].sock, &write_fds)))
            {
                // Handle incoming requests from this client
                respond_to_clients(clients[i].sock, root_paths[i]);

                // Close the socket and mark the client as inactive
                close(clients[i].sock);
                clients[i].sock = -1;
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
        ret_sockets.push_back(servers[i].sock_fd);
        root_paths.push_back(servers[i].root);
        std::cout << "listening on port => " << servers[i].port << std::endl;

    }
    return (ret_sockets);
}


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
    
    // cookie_handler(buffer);
    // if (req.Method == "POST")
    //     parse_upload_post_data(buffer);    

    //std::cout << full_path << std::endl;     
    if (!req.is_Cgi)
    {
      Response res(full_path, req.Method, req.Content_Type, client_socket, req.is_Cgi);
      this->data = res.res_to_client;
    }
    else
        this->data = Cgi_Handler(full_path, NULL);
    int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
    //close(client_socket);
    if (num_sent == -1) 
    {
        perror("Error sending data to client");
        close(client_socket);
        return;
    }
}