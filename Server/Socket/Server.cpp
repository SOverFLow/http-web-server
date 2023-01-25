#include "Server.hpp"


Server::Server()
{
    address_len = sizeof(address);
    do_socket(AF_INET, SOCK_STREAM, 0);
    do_bind_socket();
    do_listen_socket();
    do_connect();
}

void Server::do_socket(int domain, int type, int protocol)
{
    if ((server_fd = socket(domain, type, protocol)) == 0)
    {
        perror("in socket");
        exit(EXIT_FAILURE);
    }
}


void Server::do_bind_socket()
{
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    memset(address.sin_zero, '\0', sizeof(address.sin_zero));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("in bind");
        exit(EXIT_FAILURE);
    }
}
 

void Server::do_listen_socket()
{
    if (listen(server_fd, MAX_CLIENT) < 0)
    {
        perror("in listen");
        exit(EXIT_FAILURE);  
    }
}

void Server::do_connect()
{
    std::string buff;
    while (1337)
    {
        std::cout << "Waiting for new connection...." << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len))< 0)
        {
            perror("in accept");
            exit(EXIT_FAILURE);
        }
        read_bytes = recv(new_socket, (void *)buffer.data(), BODY_SIZE, 0);
        buff = buffer.data();
        Request req(buff);
        Response res(req.Path,req.Method, req.Content_Type, new_socket);
        data = res.res_to_client;
    
        send(new_socket, data.data(), data.length(), 0);
        close(new_socket);
    }
}
