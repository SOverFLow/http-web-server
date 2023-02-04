#include "Server.hpp"

Server::Server(char **env)
{
    address_len = sizeof(address);
    server_env = env;
    setup_server(AF_INET, SOCK_STREAM, 0);
}

void Server::setup_server(int domain, int type, int protocol)
{
    int option = 1;
    if ((server_fd = socket(domain, type, protocol)) == 0)
    {
        perror("in socket");
        exit(EXIT_FAILURE);
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    memset(address.sin_zero, '\0', sizeof(address.sin_zero));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("in bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENT) < 0)
    {
        perror("in listen");
        exit(EXIT_FAILURE);  
    }

}


void Server::handel_connection(int new_socket)
{
    std::string buff;
    std::string said;
    read_bytes = recv(new_socket, (void *)buffer.data(), BODY_SIZE, 0);
    buff = buffer.data();
    Request req(buff);
    Response res(req.Path, req.Method, req.Content_Type, new_socket, req.is_Cgi);
    data = res.res_to_client;
    send(new_socket, data.data(), data.length(), 0);
    close(new_socket);
}

void Server::client_connect()
{
    fd_set curent_socket, ready_socket;
    FD_ZERO(&curent_socket);
    FD_SET(server_fd, &curent_socket);
    while (1337)
    {
        ready_socket = curent_socket;
        if (select(FD_SETSIZE, &ready_socket, NULL, NULL, NULL) < 0)
        {
            perror("error in select");
            exit(EXIT_FAILURE);
        }
        for (int i =0; i < FD_SETSIZE;i++)
        {
            if (FD_ISSET(i, &ready_socket))
            {
                if (i == server_fd)
                {
                    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len))< 0)
                    {
                        perror("in accept");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(new_socket, &curent_socket);
                }
                else
                {
                    std::cout << "Waiting for new connection...." << i << std::endl;
                    handel_connection(i);
                    FD_CLR(i, &curent_socket);
                }
            }
        }
    }
}
