#include "socket/client.hpp"

client::client()
{
    sock = 0;
    msg = "hello from client";
    client_socket(AF_INET, SOCK_STREAM, 0);
    init_socket_connection();
    send_request();
}


void client::client_socket(int domain, int type, int protocol)
{
    if ((sock = socket(domain, type, protocol)) < 0)
    {
        perror("Socket creation error\n");
        exit(EXIT_FAILURE);
    }
}


void client::init_socket_connection()
{
    memset(&server_address, '0', sizeof(server_address));
    
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr)<=0)
    {
       perror("Invalid address/ Address not supported\n");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Connection failed\n");
        exit(EXIT_FAILURE);
    }
}

void client::send_request()
{
    send(sock, msg, strlen(msg), 0);
    std::cout << "message sent" << std::endl;
    vr = read(sock, buffer, 1024);
    std::cout << buffer << std::endl;
}

int main()
{
    client client;
    return (0);
}