#include "Response.hpp"
#include "../Request/Request.hpp"

Response::Response(std::string Path, std::string method, std::string contentType, int new_socket, bool is_cgi)
{
    socket_fd = new_socket;
    std::string file_content;

    if (is_cgi)
    {
        std::cout << "dkhelt " << std::endl;
        res_to_client = Cgi_Handler("/Users/salamane/Desktop/webserv/index.php", NULL);
        std::cout << res_to_client << std::endl;
    }
    else if (method == "No")
    {
        res_to_client = "HTTP/1.1 405 Method Not Allowed\r\nContent-type: text/html\r\n\r\n";
        file_content = read_file_content("/Error_Pages/405.html");
        res_to_client += file_content;
    }

    else if (method == "GET")
    {
        res_to_client = handle_get_request(Path, contentType);
    }
    else if (method == "POST")
    {
        res_to_client = handle_get_request(Path, contentType);
        //res_to_client = handle_post_request(Path, contentType);
        //std::cout << "Post request" << std::endl;
    }
    else if (method == "DELETE")
    {
        //res_to_client = handle_delete_request(Path, contentType);
        //std::cout << "Delete request" << std::endl;
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


    size_t next_pos = Path.find('?');
    if (next_pos != std::string::npos) 
        Path = Path.substr(0, next_pos);
    if (file)
    {
        if (Path.find(".", 0) == std::string::npos)
        {
            if (Path.find("/", 1) == std::string::npos)
                Path = Path + "/";
            std::string s = Path + "index.html";
            std::ifstream index(s.substr(1), std::ios::binary);
            if (index)
            {
                response = check_request_path(s) + contentType + "\r\n\r\n";
                send(socket_fd, response.data(), response.length(), 0);
                file_content = read_file_content(s);
                send(socket_fd, file_content.data(), file_content.length(), 0);
                return (res);
            }
        }
        else
        {
            response = check_request_path(Path) + contentType + "\r\n\r\n";
            send(socket_fd, response.data(), response.length(), 0);
            file_content = read_file_content(Path);
            send(socket_fd, file_content.data(), file_content.length(), 0);
            return (res);
        }
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
    {
        Status = 200;
        header = "HTTP/1.1 200 OK\r\nContent-type: ";
    }
    else
    {
        Status = 404;
        header = "HTTP/1.1 404 Not Found\r\nContent-type: ";
    }
    return (header);
}