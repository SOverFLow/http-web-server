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


class Server
{
    public:
        Server(Config config);
    private:
        int server_fd;
        int new_socket;
        long read_bytes;
        long body_size;
        int max_client;
        int server_port;
        struct sockaddr_in address;
        int address_len;
        std::string data;
        std::string buffer;
        char **server_env;
        std::vector<int> servers;
        void setup_server(int domain, int type, int protocol);
        void log_error(int result, std::string message);
        void handel_connection(int new_socket);
        void client_connect();
};

void parse_upload_post_data(std::string http_request);
