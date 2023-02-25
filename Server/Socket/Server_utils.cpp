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



void check_if_path_is_directory(std::string full_path, std::string req_path, int client_socket)
{
    //std::cout << "full_path: " << full_path <<std::endl;
    //std::cout << "req_path: " << req_path <<std::endl;
    // Check if the request path corresponds to a directory
    struct stat path_stat;
    std::string all = "/Users/selhanda/Desktop/webserv" + full_path;
    if (stat( all.c_str(), &path_stat) == 0 && S_ISDIR(path_stat.st_mode))
    {
        // if (req_path.back() != '/')
        // {
        //     // If the path doesn't end in a trailing slash, redirect to the same path with a trailing slash
        //     std::string redirect_path = req_path + "/";
        //     std::string response = "HTTP/1.1 301 Moved Permanently\r\nLocation: " + redirect_path + "\r\n\r\n";
        //     send(client_socket, response.c_str(), response.length(), 0);
        //     close(client_socket);
        //     return;
        // }

        // else
        // {
            //std::cout << "Im in" << std::endl;
            // Generate an HTML document listing the files and subdirectories in the directory
            std::cout << "yes" << std::endl;
            std::stringstream response;
            response << "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            response << "<html>\n<head>\n<title>Index of " << req_path << "</title>\n</head>\n<body>\n<h1>Index of " << req_path << "</h1>\n<hr />\n<ul>\n";
            DIR *dir;
            struct dirent *ent;
            std::cout << "Directory =>" << full_path << std::endl;
            if ((dir = opendir(all.c_str())) != NULL)
            {
                std::cout << "yes 2" << std::endl;
                while ((ent = readdir(dir)) != NULL)
                {
                    std::string filename = ent->d_name;
                    if (filename != "." && filename != "..")
                    {
                        std::string path = filename;
                        // std::cout << "path = " << path << std::endl;
                         std::cout << "full_path = " << full_path << std::endl;
                        if (S_ISDIR(ent->d_type))
                        {
                            response << "<li><a href=\"" <<  path << "/\">" << filename << "/</a></li>\n";
                        }
                        else
                        {

                            response << "<li><a href=\"" << path << "\">" << filename << "</a></li>\n";
                        }
                    }
                }
                closedir(dir);
            }
            response << "</ul>\n<hr />\n</body>\n</html>\n";
            std::string response_str = response.str();
            send(client_socket, response_str.c_str(), response_str.length(), 0);
            close(client_socket);
            return;
        // }
    }
}