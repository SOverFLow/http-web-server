#include "Response.hpp"


Response::Response(std::string Path, std::string method, std::string contentType, int new_socket)
{
    socket_fd = new_socket;
    if (method == "GET")
    {
        res_to_client = handle_get_request(Path, contentType);
    }
    else if (method == "POST")
    {
        //res_to_client = handle_post_request(Path, contentType);
        std::cout << "Post request" << std::endl;
    }
    else if (method == "DELETE")
    {
        //res_to_client = handle_delete_request(Path, contentType);
        std::cout << "Delete request" << std::endl;
    }
}

void Response::handle_cgi_request(std::string cgi_path, std::string query)
{
    setenv("QUERY", query.data(), 1);
    setenv("METHOD", "GET", 1);

    int pid = fork();
    if (pid == 0)
    {
        execve(cgi_path.data(), (char *const *)cgi_path.data(), NULL);
        exit(0);
    }
    else
        waitpid(pid,NULL,0);
}

std::string Response::read_file_content(std::string Path)
{
    std::ifstream file(Path.substr(1), std::ios::binary);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_contents = buffer.str();
    return (file_contents);
}

std::string Response::handle_get_request(std::string Path, std::string contentType)
{
    std::ifstream file(Path.substr(1), std::ios::binary);
    std::string file_content;
    std::string res;

    if (file)
    {
        response = check_request_path(Path) + contentType + "\r\n\r\n";
        send(socket_fd, response.data(), response.length(), 0);
        file_content = read_file_content(Path);
        send(socket_fd, file_content.data(), file_content.length(), 0);
        return (res);
    }
    else if (Path == "/")
    {
        res = check_request_path("/index.html") + contentType + "\r\n\r\n";
        file_content = read_file_content("/index.html");
        res += file_content; 
    }
    else
    {
        res = check_request_path(Path) + "text/html" + "\r\n\r\n";
        file_content = read_file_content("/Error_Pages/404.html");
        res += file_content;
    }
    return (res);
}

std::string Response::check_request_path(std::string Path)
{
    std::string header;
    std::ifstream file(Path.substr(1), std::ios::binary);
    if (file)
        header = "HTTP/1.1 200 OK\r\nContent-type: ";
    else
        header = "HTTP/1.1 404 Not Found\r\nContent-type: ";
    return (header);
}