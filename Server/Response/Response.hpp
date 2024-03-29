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
#include <sys/stat.h>
#include <dirent.h>
#include <map>

class Response
{
private:
   std::string response;
   int socket_fd;
   int Status;
   std::vector<std::string> index_files;
   bool auto_index;
   std::string full_path;
   std::string req_path;
   std::string server_cookies;
   bool is_location;
   bool is_header_send;
   std::map<std::string, std::string> error_pages;
public:
    Response(std::string Path, std::string method, std::string contentType, int new_socket, 
    bool is_cgi, std::vector<std::string> indexs, bool autoindex,
    std::string full_path, std::string req_path, bool is_location, std::string cookies_part, std::map<std::string, std::string> error_pages);
    std::string check_request_path(std::string Path);
    std::string read_file_content(std::string Path);
    int handle_get_request(std::string Path, std::string contentType);
    int	handel_delete_request(std::string file_path);
    int serve_index(std::string Path, std::string contentType);
    int serve_other_files(std::string Path, std::string contentType);
    int serve_root_path(std::string Path, std::string contentType);
    std::string res_to_client;
    int num_sent;
};

void   cookie_handler(std::string http_request);
std::string get_index_file_name(std::vector<std::string> index, std::string path);
int		CheckIsFile(const std::string& path);
int serve_auto_index(std::string full_path, std::string req_path, int client_socket);

#endif