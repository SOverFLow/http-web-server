#include "socket/server.hpp"


server::server()
{
    address_len = sizeof(address);
    do_socket(AF_INET, SOCK_STREAM, 0);
    do_bind_socket();
    do_listen_socket();
    do_connect();
}

void server::do_socket(int domain, int type, int protocol)
{
    if ((server_fd = socket(domain, type, protocol)) == 0)
    {
        perror("in socket");
        exit(EXIT_FAILURE);
    }
}


void server::do_bind_socket()
{
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof (address)) < 0)
    {
        perror("in bind");
        exit(EXIT_FAILURE);
    }
}


void server::do_listen_socket()
{
    if (listen(server_fd,10) < 0)
    {
        perror("in listen");
        exit(EXIT_FAILURE);
    }
}

void server::do_connect()
{
    char *response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    while (1337)
    {
        std::cout << std::endl;
        std::cout << "Waiting for new connection...." << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len))<0 )
        {
         perror("in accept");
            exit(EXIT_FAILURE);
        }
        char buffer[30000] = {0};
        rv = read(new_socket, buffer, 30000);
        std::cout << buffer << std::endl;
        write(new_socket, response, strlen(response));
        std::cout << " message sent" << std::endl;
        close(new_socket);
    }
}


int main()
{
    server webserv;
    return (0);
}