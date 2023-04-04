#include "Server.hpp"
#include <vector>
#include <fstream>

int Server::parse_upload_post_data(std::string full_request, std::string body, std::string upload_path, int connfd, size_t content_length, int bytes_received, char *buffer)
{
    //std::cout << body << std::endl;
    char new_buffer[1024];
    size_t total_bytes_received = 0;
     while (total_bytes_received < content_length) {
            //set_nonblocking(connfd);
            //std::cout << content_length << std::endl;
            bytes_received = recv(connfd, new_buffer, 1024, 0);
            if (bytes_received == 0)
                break;
            if (bytes_received == -1)
                break;
            if (bytes_received != -1)
            {
                full_request += std::string(new_buffer, bytes_received);
                total_bytes_received += bytes_received;
                std::cout << "bytes: " << total_bytes_received << std::endl;
            }

    }
    // (void)connfd;
    (void)buffer;
    // (void)content_length;
    // (void)bytes_received;
    // full_request = body;
    std::string data(full_request);
    std::string boundary("boundary=");
    (void)body;

    size_t pos = data.find(boundary);
    // if (pos == std::string::npos)
    //     return (0);

    std::string boundary_value = "--" + data.substr(pos + boundary.length(), 16);

    
    std::vector<std::string> parts;
    size_t prev_pos = data.find(boundary_value);
    while (prev_pos != std::string::npos) {
        size_t next_pos = data.find(boundary_value, prev_pos + boundary_value.length());
        if (next_pos == std::string::npos) {
            parts.push_back(data.substr(prev_pos));
            break;
        }
        parts.push_back(data.substr(prev_pos, next_pos - prev_pos));
        prev_pos = next_pos;
    }


    for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it)
    {
        std::string content_type_header("Content-Type:");
        std::string content_disposition_header("Content-Disposition:");
        size_t content_type_pos = it->find(content_type_header);
        size_t content_disposition_pos = it->find(content_disposition_header);
        if (content_type_pos == std::string::npos || content_disposition_pos == std::string::npos) {
            continue;
        }

    
        std::string content_type_value = it->substr(content_type_pos + content_type_header.length(), it->find("\r\n", content_type_pos) - (content_type_pos + content_type_header.length()));
        std::string content_disposition_value = it->substr(content_disposition_pos + content_disposition_header.length(), it->find("\r\n", content_disposition_pos) - (content_disposition_pos + content_disposition_header.length()));

        std::string filename_header("filename=\"");
        size_t filename_pos = content_disposition_value.find(filename_header);
        if (filename_pos == std::string::npos) {
            continue;
        }
        std::string filename_value = content_disposition_value.substr(filename_pos + filename_header.length(), content_disposition_value.find("\"", filename_pos + filename_header.length()) - (filename_pos + filename_header.length()));
        size_t file_data_pos = it->find("\r\n\r\n") + 4;
        std::string file_data = it->substr(file_data_pos, it->length() - file_data_pos - boundary_value.length());




        if (filename_value.empty())
            return (0);
  
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        std::string directory = cwd + upload_path;

        std::fstream dir(directory.c_str());
     
        if (dir.is_open() == false)
        {
            if (mkdir(directory.c_str(), 0777) == 0)
            {
                std::cout << "Directory created successfully" << std::endl;
            }
        }
      
        
        std::ofstream outfile(directory + filename_value, std::ios::binary);
       

        outfile.write(file_data.c_str(), file_data.length());
        outfile.close();


        // if (total_bytes_received == content_length)
        //     return (1);
        // else
        //     return (-1);

    }
    return (1);
}

