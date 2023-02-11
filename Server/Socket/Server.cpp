#include "Server.hpp"

Server::Server(char **env)
{
    this->address_len = sizeof(this->address);
    this->server_env = env;
    setup_server(AF_INET, SOCK_STREAM, 0);
}

void Server::setup_server(int domain, int type, int protocol)
{
    int option = 1;
    if ((this->server_fd = socket(domain, type, protocol)) == 0)
    {
        perror("in socket");
        exit(EXIT_FAILURE);
    }
    setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(PORT);
    memset(this->address.sin_zero, '\0', sizeof(this->address.sin_zero));


    if (bind(this->server_fd, (struct sockaddr *)&address, sizeof(this->address)) < 0)
    {
        perror("in bind");
        exit(EXIT_FAILURE);
    }

    if (listen(this->server_fd, MAX_CLIENT) < 0)
    {
        perror("in listen");
        exit(EXIT_FAILURE);  
    }

}


void Server::handel_connection(int new_socket)
{
    
    std::string buff;
    this->read_bytes = recv(new_socket, (void *)buffer.data(), BODY_SIZE, 0);
    buff = buffer.data();
    Request req(buff);
    if (!req.is_Cgi)
    {
        Response res(req.Path, req.Method, req.Content_Type, new_socket, req.is_Cgi);
        this->data = res.res_to_client;
    }
    send(new_socket, this->data.data(), this->data.length(), 0);
    close(new_socket);
}


void Server::client_connect()
{
    std::string  str;
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
                if (i == this->server_fd)
                {
                    if ((this->new_socket = accept(this->server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len))< 0)
                    {
                        perror("in accept");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(this->new_socket, &curent_socket);
                }
                else
                {
                    std::cout << "Waiting for new connection...." << i << std::endl;
                    this->data = Cgi_Handler("index.php", NULL);
                    handel_connection(i);
                    FD_CLR(i, &curent_socket);
                }
            }
        }
    }
}
