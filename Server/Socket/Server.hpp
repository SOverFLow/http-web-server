#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

#define PORT 8080
#define BODY_SIZE 30000
#define MAX_CLIENT 10


class Server
{
    public:
        Server();
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
        void do_connect();
};
