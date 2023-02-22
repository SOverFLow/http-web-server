#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>

#include "ServerBlock.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Cgi_Handler.hpp"

#define MAX_EVENTS 10

class Server
{
private:
    std::vector<int> sockets;
    std::vector<Client> clients;
    std::vector<std::string> root_paths;
    std::string data;

public:
    Server(Config config);
    void connection(std::vector<ServerBlock> &servers);
    void clients_accept(std::vector<int> &sockets, std::vector<Client> &clients);
    void respond_to_clients(int client_socket, std::string root_path);
};

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::vector<int> setup_sockets(std::vector<ServerBlock> &servers)
{
    std::vector<int> ret_sockets;

    for (int i =0; i < servers.size(); i++)
    {
        struct sockaddr_in address;
        int option = 1;
        int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd == 0)
        {
            std::cout << "error in the socket" << std::endl;
            exit(1);
        }
        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(servers[i].port);

        if (bind(sock_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            std::cout << "error in binding" << std::endl;
            exit(1);
        }


        if (listen(sock_fd, 10) < 0)
        {
            std::cout << "error in listening" << std::endl;
            exit(1);  
        }
        set_nonblocking(sock_fd);
        ret_sockets.push_back(sock_fd);
        std::cout << "listening on port => " << servers[i].port << std::endl;

    }
    return (ret_sockets);
}

void handle_new_connection(int listening_socket, std::vector<pollfd> &fds) {
    int client_socket = accept(listening_socket, NULL, NULL);
    if (client_socket < 0) {
        perror("Error accepting new connection");
        return;
    }
    set_nonblocking(client_socket);
    fds.push_back(pollfd{client_socket, POLLIN, 0});
}

void handle_client_request(int client_socket, std::vector<Client> &clients, std::vector<std::string> &root_paths) {
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
    full_path = root_paths[req.server_index] + req.Path.substr(1);
       
