#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>

struct ServerInfo {
    int sock_fd;
    std::string root;
};

struct ClientInfo {
    int sock_fd;
    struct sockaddr_in address;
    socklen_t address_len;
    std::string data;
};

std::vector<ServerInfo> setup_servers() {
    std::vector<ServerInfo> servers;
    // setup each server and add it to the vector
    return servers;
}

std::vector<pollfd> create_pollfds(std::vector<ServerInfo>& servers) {
    std::vector<pollfd> pollfds;
    for (auto& server : servers) {
        pollfd pfd = {server.sock_fd, POLLIN, 0};
        pollfds.push_back(pfd);
    }
    return pollfds;
}

int main() {
    // setup the servers
    std::vector<ServerInfo> servers = setup_servers();

    // create the pollfd structs
    std::vector<pollfd> pollfds = create_pollfds(servers);

    // create a vector to hold the clients
    std::vector<ClientInfo> clients;

    while (true) {
        int ready_count = poll(&pollfds[0], pollfds.size(), -1);
        if (ready_count == -1) {
            std::cout << "Error in poll" << std::endl;
            exit(1);
        }

        // loop through the pollfds to find the socket that triggered the event
        for (int i = 0; i < pollfds.size(); i++) {
            if (pollfds[i].revents & POLLIN) {
                if (pollfds[i].fd == servers[i].sock_fd) {
                    // new connection
                    struct sockaddr_in client_address;
                    socklen_t client_address_len = sizeof(client_address);
                    int client_sock = accept(servers[i].sock_fd, (struct sockaddr *)&client_address, &client_address_len);
                    if (client_sock == -1) {
                        std::cout << "Error in accept" << std::endl;
                        exit(1);
                    }
                    std::cout << "New connection from " << inet_ntoa(client_address.sin_addr) << std::endl;
                    pollfd pfd = {client_sock, POLLIN, 0};
                    pollfds.push_back(pfd);
                    ClientInfo client = {client_sock, client_address, client_address_len, ""};
                    clients.push_back(client);
                } else {
                    // client request
                    char buffer[1024];
                    int num_bytes = recv(pollfds[i].fd, buffer,
