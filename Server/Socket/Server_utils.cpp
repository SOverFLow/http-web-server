#include "Server.hpp"

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::vector<pollfd> create_pollfds(std::vector<ServerBlock>& servers) {
    std::vector<pollfd> pollfds;
    for (std::vector<ServerBlock>::iterator it = servers.begin(); it != servers.end(); ++it) 
    {
        pollfd pfd = { it->sock_fd, POLLIN, 0 };
        pollfds.push_back(pfd);
    }
    return pollfds;
}

void handle_new_connection(int listening_socket, std::vector<pollfd> &fds) {
    int client_socket = accept(listening_socket, NULL, NULL);
    if (client_socket < 0) {
        perror("Error accepting new connection");
        return;
    }
    set_nonblocking(client_socket);
    pollfd pfd = {client_socket, POLLIN, 0};
    fds.push_back(pfd);
}

int check_if_url_is_location(std::string url, std::vector<Locations> locations)
{

    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (1); 
    }
    return (0);
}


std::string get_root_location(std::string url, std::vector<Locations> locations)
{
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->root); 
    }
    return ("");
}