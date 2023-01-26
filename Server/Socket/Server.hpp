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

#define PORT 8080
#define BODY_SIZE 30000
#define MAX_CLIENT 10


class Server
{
    public:
        Server();
        void do_connect();
    private:
        int server_fd;
        int new_socket;
        long read_bytes;
        struct sockaddr_in address;
        int address_len;
        std::string data;
        std::string buffer;
        void do_socket(int domain, int type, int protocol);
        void do_bind_socket();
        void do_listen_socket();
        void do_handel_connection(int new_socket);
};
