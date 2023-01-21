#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <iostream>

#define PORT 8080

class client
{
private:

public:
    int sock ;
    long vr;
    struct sockaddr_in server_address;
    char *msg;
    char buffer[1024] = {0};

    client();
    void client_socket(int domain, int type, int protocol);
    void init_socket_connection();
    void send_request();

};

