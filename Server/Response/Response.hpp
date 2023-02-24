#ifndef RESPONSE_HPP

#define RESPONSE_HPP

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

class Response
{
private:
   std::string response;
   int socket_fd;
   int Status;
   std::vector<std::string> index_files;
public:
    Response(std::string Path, std::string method, std::string contentType, int new_socket, bool is_cgi, std::vector<std::string> indexs);
    std::string check_request_path(std::string Path);
    std::string read_file_content(std::string Path);
    std::string handle_get_request(std::string Path, std::string contentType);
    void handle_cgi_request(std::string cgi_path, std::string query);
    std::string	handel_delete_request(std::string file_path);
    void serve_index(std::string Path, std::string contentType);
    void serve_other_files(std::string Path, std::string contentType);
    std::string res_to_client;
};

void   cookie_handler(std::string http_request);

#endif