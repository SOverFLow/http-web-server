#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>

class Response
{
private:
   std::string response;
   int socket_fd;
public:
    Response(std::string Path, std::string method, std::string contentType, int new_socket);
    std::string check_request_path(std::string Path);
    std::string read_file_content(std::string Path);
    std::string handle_get_request(std::string Path, std::string contentType);
};
