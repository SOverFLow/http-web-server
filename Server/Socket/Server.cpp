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
    this->sockets = setup_sockets(servers);

    while(1)
    {
        std::cout << "Connection" << std::endl;
        clients_accept(sockets, clients);
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




void Server::clients_accept(std::vector<int> &sockets, std::vector<Client> &clients)
{
    for (int i = 0; i < sockets.size(); i++)
    {
        Client new_client;
        new_client.sock = accept(sockets[i], (struct sockaddr *)&new_client.address, (socklen_t*)&new_client.address_len);
        if (new_client.sock < 0)
        {
            std::cout << "Error in accept" << std::endl;
            exit(1);
        }
        clients.push_back(new_client);
        // std::cout << root_paths[i] << std::endl;
        respond_to_clients(new_client.sock, root_paths[i]);
    }
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





// void Server::handel_connection(int new_socket) 
// {
//   char buffer[this->body_size];
//   int num_bytes = recv(new_socket, buffer, sizeof(buffer), 0);
    
//     if (num_bytes == -1) 
//     {
//         std::cout<< "socket => " << new_socket << std::endl;
//         perror("Error receiving data from client");
//         close(new_socket);
//         return;
//     } 
//     else if (num_bytes == 0) 
//     {
//         close(new_socket);
//         return;
//     }
//     Request req(buffer);
//     //std::cout << buffer << std::endl;

//     //std::cout << "here=>" << std::endl;
//     cookie_handler(buffer);
//     if (req.Method == "POST")
//         parse_upload_post_data(buffer);
//     if (!req.is_Cgi)
//     {
//       Response res(req.Path, req.Method, req.Content_Type, new_socket, req.is_Cgi);
//       this->data = res.res_to_client;
//     }
//     else
//         this->data = Cgi_Handler(req.Path.substr(1), NULL);
//     int num_sent = send(new_socket, this->data.c_str(), this->data.size(), 0);
//     close(new_socket);
//     if (num_sent == -1) 
//     {
//         perror("Error sending data to client");
//         close(new_socket);
//         return;
//     }
// }


// void Server::client_connect()
// {
//     std::string  str;
//     fd_set curent_socket, ready_socket;
//     FD_ZERO(&curent_socket);
//     std::vector<int>::iterator s;
//     for (s = servers.begin(); s != servers.end(); s++)
//     {
//         FD_SET(*s, &curent_socket);
//         //std::cout << "server fd = " << *s << std::endl;
//     }
//     while (1337)
//     {
//         ready_socket = curent_socket;
//         if (select(FD_SETSIZE, &ready_socket, NULL, NULL, NULL) < 0)
//         {
//             perror("error in select");
//             exit(EXIT_FAILURE);
//         }
//         for (int i =0; i < FD_SETSIZE;i++)
//         {
//             if (FD_ISSET(i, &ready_socket))
//             {
//                 if (i == this->server_fd)
//                 {
//                     if ((this->new_socket = accept(this->server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len))< 0)
//                     {
//                         perror("in accept");
//                         exit(EXIT_FAILURE);
//                     }
//                     FD_SET(this->new_socket, &curent_socket);
//                 }
//                 else
//                 {
//                     std::cout << "listening on port => " << this->server_port << std::endl;
//                     handel_connection(i);
//                     FD_CLR(i, &curent_socket);
//                 }
//             }
//         }
//     }
// }
