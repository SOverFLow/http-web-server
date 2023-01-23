#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define PORT 8080

class server
{
    public:
        server();
    private:
        int server_fd;
        int new_socket;
        std::string method;
        std::string path;
        std::string version;
        long rv;
        struct sockaddr_in address;
        int address_len;
        void do_socket(int domain, int type, int protocol);
        void do_bind_socket();
        void do_listen_socket();
        void do_connect();
        void do_parse_http_request(std::string http_request);
        std::string do_handle_request();
        std::string get_file_type(std::string path);

};
