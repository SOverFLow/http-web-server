#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <poll.h>
#include <dirent.h>
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
        std::string tmp_path;
        std::vector<std::string> tmp_index;
        int path_check;
        std::vector<int> sockets;
        std::vector<Client> clients;
        std::vector<std::string> root_paths;
        std::vector<int> setup_sockets(std::vector<ServerBlock> &servers);
        void connection(std::vector<ServerBlock> &servers);
        void handle_client_request(int client_socket);
        void respond_to_clients(int client_socket, std::string root_path, ServerBlock server, int tmp);
};

void parse_upload_post_data(std::string http_request);
int set_nonblocking(int fd);
std::vector<pollfd> create_pollfds(std::vector<ServerBlock>& servers);
void handle_new_connection(int listening_socket, std::vector<pollfd> &fds);
int check_if_url_is_location(std::string url, std::vector<Locations> locations);
std::string get_root_location(std::string url, std::vector<Locations> locations);
std::vector<std::string> get_index_location(std::string url, std::vector<Locations> locations);
void check_if_path_is_directory(std::string full_path, std::string req_path, int client_socket);

