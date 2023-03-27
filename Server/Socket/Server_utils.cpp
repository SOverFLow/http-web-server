#include "Server.hpp"

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

std::vector<pollfd> create_pollfds(std::vector<ServerBlock>& servers) {
    std::vector<pollfd> pollfds;
    for (std::vector<ServerBlock>::iterator it = servers.begin(); it != servers.end(); ++it) 
    {
        pollfd pfd = { it->sock_fd, POLLIN, 0 };
        pollfds.push_back(pfd);
    }
    return pollfds;
}

void handle_new_connection(int listening_socket, std::vector<pollfd> &fds) {
    int client_socket = accept(listening_socket, NULL, NULL);
    if (client_socket < 0) {
        perror("Error accepting new connection");
        return;
    }
    set_nonblocking(client_socket);
    pollfd pfd = {client_socket, POLLIN, 0};
    fds.push_back(pfd);
}

int check_if_url_is_location(std::string url, std::vector<Locations> locations)
{
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (1); 
    }
    return (0);
}


std::string get_root_location(std::string url, std::vector<Locations> locations)
{
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->root); 
    }
    return ("");
}

Locations get_location(std::string url, std::vector<Locations> locations)
{
    std::vector<Locations>::iterator it = locations.begin();
    for (; it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (*it); 
    }
    return (*it);
}


std::vector<std::string> get_index_location(std::string url, std::vector<Locations> locations)
{
    std::vector<std::string> indexs;
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->index); 
    }
    return (indexs); 
}

bool Check_is_method_allowed(std::string method, std::vector<std::string> allowed_methods)
{
    for (std::vector<std::string>::iterator it = allowed_methods.begin(); it != allowed_methods.end(); ++it) 
    {
        if (method == *it)
            return (true); 
    }
    return (false);
}

std::vector<std::string> get_allowed_methods(std::string url, std::vector<Locations> locations)
{
    std::vector<std::string> allowed_methods;
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->allowed_method); 
    }
    return (allowed_methods); 
}

std::string Server::Return_Error_For_Bad_Request(int status)
{
    std::string response;
    std::string header;
    std::string file_content;

    if (status == 501)
    {
        header = "HTTP/1.1 501 Not Implemented\r\nContent-type: text/html\r\n" + this->cookies_part + "\r\n";
        file_content = Return_File_Content(this->error_pages["501"]);
        response = header + file_content;
    }
    else if (status == 400)
    {
        header = "HTTP/1.1 400 Bad Request\r\nContent-type: text/html\r\n" + this->cookies_part + "\r\n";
        file_content = Return_File_Content(this->error_pages["400"]);
        response = header + file_content;
    }
    else if (status == 414)
    {
        header = "HTTP/1.1 414 URI Too Long\r\nContent-type: text/html\r\n" + this->cookies_part + "\r\n";
        file_content = Return_File_Content(this->error_pages["414"]);
        response = header + file_content;
    }
    else if (status == 413)
    {
        header = "HTTP/1.1 413 Content Too Large\r\nContent-type: text/html\r\n" + this->cookies_part + "\r\n";
        file_content = Return_File_Content(this->error_pages["413"]);
        response = header + file_content;
    }

    return (response);
}

std::string Return_File_Content(std::string Path)
{
    std::ifstream file(Path.substr(1), std::ios::binary);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_contents = buffer.str();
    return (file_contents);
}

std::string get_redirect_url_for_location(std::string url, std::vector<Locations> locations)
{
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->redirect_url); 
    }
    return ("");
}


int get_redirect_code_for_location(std::string url, std::vector<Locations> locations)
{
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->redirect_code); 
    }
    return (0);
}


bool check_if_location_has_redirect(std::string url, std::vector<Locations> locations)
{
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->redirect);
    }
    return (false);
}

bool Check_Cgi_Location_Status(std::string url, std::vector<Locations> locations)
{
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->CgiStatus);
    }
    return (false);
}


bool Check_upload_Location_Status(std::string url, std::vector<Locations> locations)
{
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->uploadable);
    }
    return (false);
}

std::string Get_upload_Location_Path(std::string url, std::vector<Locations> locations)
{
    for (std::vector<Locations>::iterator it = locations.begin(); it != locations.end(); ++it) 
    {
        if (url == it->Name)
            return (it->uploadPath);
    }
    return ("");
}

std::string get_index_file_name_cgi(std::vector<std::string> index, std::string path)
{
    std::vector<std::string>::iterator it;
    for (it = index.begin(); it != index.end(); ++it) 
    {
        std::ifstream file(path.substr(1) + *it, std::ios::binary);
        if (file)
            return (path.substr(1) + *it);
        else
            return (path.substr(1) + *++it);
    }
    return ("no");
}



std::string serve_index_for_cgi(std::string Path, std::vector<std::string> index_files)
{
    std::string correct_index;
    correct_index = get_index_file_name_cgi(index_files, Path);
    return (correct_index);
}

std::string return_redirect_msg(int code)
{
    std::string msg;
    if (code == 301)
        msg = "Moved Permanently";
    else if (code == 302)
        msg = "Found";
    else if (code == 303)
        msg = "See Other";
    else if (code == 304)
        msg = "Not Modified";
    else if (code == 307)
        msg = "Temporary Redirect";
    else if (code == 308)
        msg = "Permanent Redirect";
    return (msg);
}