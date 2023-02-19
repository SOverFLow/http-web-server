#include "Response.hpp"
#include "../Request/Request.hpp"

Response::Response(std::string Path, std::string method, std::string contentType, int new_socket, bool is_cgi)
{
    this->socket_fd = new_socket;
    std::string file_content;

    if (method == "No")
    {
        this->res_to_client = "HTTP/1.1 405 Method Not Allowed\r\nContent-type: text/html\r\n\r\n";
        file_content = read_file_content("/Error_Pages/405.html");
        this->res_to_client += file_content;
    }

    else if (method == "GET")
    {
        this->res_to_client = handle_get_request(Path, contentType);
    }
    else if (method == "POST")
    {
        this->res_to_client = handle_get_request(Path, contentType);
    }
    else if (method == "DELETE")
        this->res_to_client = handel_delete_request(Path);
}

std::string Response::read_file_content(std::string Path)
{
    std::ifstream file(Path.substr(1), std::ios::binary);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_contents = buffer.str();
    return (file_contents);
}

std::string Response::check_request_path(std::string Path)
{
    std::string header;
    std::ifstream file(Path.substr(1), std::ios::binary);
    if (file)
    {
        this->Status = 200;
        header = "HTTP/1.1 200 OK\r\nContent-type: ";
    }
    else
    {
        this->Status = 404;
        header = "HTTP/1.1 404 Not Found\r\nContent-type: ";
    }
    return (header);
}