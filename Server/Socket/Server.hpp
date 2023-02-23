#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../../Config/Config.hpp"

class Client
{
    public:
        Client();
	    socklen_t				address_len;
	    struct sockaddr_in	address;
	    int						sock;
};


class Server
{
    public:
        Server(Config config);
    private:
        std::string data;
        int max_socket;
        std::vector<int> sockets;
        std::vector<Client> clients;
        std::vector<std::string> root_paths;
        std::vector<int> setup_sockets(std::vector<ServerBlock> &servers);
        void connection(std::vector<ServerBlock> &servers);
        //void handle_new_connection(int server_socket);
        void handle_client_request(int client_socket);
        void clients_accept(std::vector<int> &sockets, std::vector<Client> &clients);
        void respond_to_clients(int client_socket, std::string root_path);
};

void parse_upload_post_data(std::string http_request);
