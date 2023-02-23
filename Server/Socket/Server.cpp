#include "Server.hpp"


Server::Server(Config config)
{
    connection(config.Servers);
}


Client::Client()
{

}


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



void Server::connection(std::vector<ServerBlock> &servers)
{
    this->sockets = setup_sockets(servers);
    std::vector<pollfd> pollfds = create_pollfds(servers);
    std::vector<Client> clients;

    int tmp = 0;
    while(1)
    {
        int ready_count = poll(&pollfds[0], pollfds.size(), -1);
        if (ready_count == -1) {
            std::cout << "Error in poll" << std::endl;
            exit(1);
        }

        for (int i = 0; i < pollfds.size(); i++) {
            if (pollfds[i].revents & POLLIN) {
                if (pollfds[i].fd == servers[i].sock_fd) 
                {
                   
                    handle_new_connection(servers[i].sock_fd, pollfds);
                    tmp = i;
                }
                 else 
                 {
                    respond_to_clients(pollfds[i].fd, root_paths[tmp]);
                 }
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
        set_nonblocking(servers[i].sock_fd);
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
    // std::cout << full_path << std::endl;
    
    // cookie_handler(buffer);
    // if (req.Method == "POST")
    //     parse_upload_post_data(buffer);    

    // std::cout << full_path << std::endl;     
    if (!req.is_Cgi)
    {
      Response res(full_path, req.Method, req.Content_Type, client_socket, req.is_Cgi);
      this->data = res.res_to_client;
    }
    else
    {
        int len = full_path.length();
        if(full_path[len - 1] == '/')
            full_path = full_path + "index.php";
        this->data = Cgi_Handler(full_path, NULL);
    }
    int num_sent = send(client_socket, this->data.c_str(), this->data.size(), 0);
    close(client_socket);
    if (num_sent == -1) 
    {
        perror("Error sending data to client");
        close(client_socket);
        return;
    }
}

