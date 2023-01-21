#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>

#define PORT 8080

class server
{
    private:
    public:
        int server_fd;
        int new_socket;
        long rv;
        struct sockaddr_in address;
        int address_len;
        server();
        void do_socket(int domain, int type, int protocol);
        void do_bind_socket();
        void do_listen_socket();
        void do_connect();

};
