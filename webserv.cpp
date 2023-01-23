#include "socket/server.hpp"


server::server()
{
    address_len = sizeof(address);
    do_socket(AF_INET, SOCK_STREAM, 0);
    do_bind_socket();
    do_listen_socket();
    do_connect();
}


void server::do_parse_http_request(std::string http_request)
{
    size_t pos = 0;
    size_t next_pos = http_request.find(' ');
    if (next_pos != std::string::npos) {
        method = http_request.substr(0, next_pos);
        pos = next_pos + 1;
    }
    next_pos = http_request.find(' ', pos);
    if (next_pos != std::string::npos) {
        path = http_request.substr(pos, next_pos - pos);
        pos = next_pos + 1;
    }
    next_pos = http_request.find('\r', pos);
    if (next_pos != std::string::npos) {
        version = http_request.substr(pos, next_pos - pos);
    }
}

std::string server::do_handle_request()
{
    std::cout << path.substr(1) << std::endl;
    std::ifstream file(path.substr(1), std::ios::binary);

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_contents = buffer.str();
    std::string response = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
    if (file) {
        response += file_contents;
    } else if (path == "/") {
        std::ifstream file("index.html", std::ios::binary);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string file_contents = buffer.str();
        response += file_contents;
    } else {
        std::ifstream file("404.html", std::ios::binary);
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string file_contents = buffer.str();
        response = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
        response += file_contents;
    }
    return response;
}


void server::do_socket(int domain, int type, int protocol)
{
    if ((server_fd = socket(domain, type, protocol)) == 0)
    {
        perror("in socket");
        exit(EXIT_FAILURE);
    }
}


void server::do_bind_socket()
{
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof (address)) < 0)
    {
        perror("in bind");
        exit(EXIT_FAILURE);
    }
}
 

void server::do_listen_socket()
{
    if (listen(server_fd,10) < 0)
    {
        perror("in listen");
        exit(EXIT_FAILURE);  
    }
}

void server::do_connect()
{
    std::string data;
    char buffer[30000];

    while (1337)
    {
        std::cout << std::endl;
        std::cout << "Waiting for new connection...." << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len))<0 )
        {
            perror("in accept");
            exit(EXIT_FAILURE);
        }
        rv = recv(new_socket, buffer, 30000, 0);
        do_parse_http_request(buffer);
        data = do_handle_request();
        const char *response_data = data.c_str();
        send(new_socket, response_data, strlen(response_data), 0);
        close(new_socket);
    }
}


int main()
{
    server webserv;
    return (0);
}