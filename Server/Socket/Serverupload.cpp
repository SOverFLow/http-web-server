#include "Server.hpp"
#include <vector>


void Server::parse_upload_post_data(char * buffer) {
    std::string data(buffer);
    std::string boundary("boundary=");

    // std::cout << "=========================" << std::endl;
    // std::cout << data << std::endl;
    // std::cout << "=========================" << std::endl;


    // Find the boundary string in the request
    size_t pos = data.find(boundary);
    // if (pos == std::string::npos) {
    //     std::cout << " boundary not found" << std::endl;
    //     return;
    // }

    // Get the boundary value
    std::string boundary_value = "--" + data.substr(pos + boundary.length(), 16);


    // std::cout << "=========================" << std::endl;
    // std::cout << boundary_value << std::endl;
    // std::cout << "=========================" << std::endl;

    // Split the request data by the boundary
    
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

    // Parse the parts

    for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end(); ++it)
    {
        // Find the content-type and content-disposition headers

        std::string content_type_header("Content-Type:");
        std::string content_disposition_header("Content-Disposition:");
        size_t content_type_pos = it->find(content_type_header);
        size_t content_disposition_pos = it->find(content_disposition_header);
        if (content_type_pos == std::string::npos || content_disposition_pos == std::string::npos) {
            continue;
        }

        // Parse the content-type and content-disposition headers

        std::string content_type_value = it->substr(content_type_pos + content_type_header.length(), it->find("\r\n", content_type_pos) - (content_type_pos + content_type_header.length()));
        std::string content_disposition_value = it->substr(content_disposition_pos + content_disposition_header.length(), it->find("\r\n", content_disposition_pos) - (content_disposition_pos + content_disposition_header.length()));



        // Find the file name and file data in the part
        std::string filename_header("filename=\"");
        size_t filename_pos = content_disposition_value.find(filename_header);
        if (filename_pos == std::string::npos) {
            continue;
        }
        std::string filename_value = content_disposition_value.substr(filename_pos + filename_header.length(), content_disposition_value.find("\"", filename_pos + filename_header.length()) - (filename_pos + filename_header.length()));
        size_t file_data_pos = it->find("\r\n\r\n") + 4;
        std::string file_data = it->substr(file_data_pos, it->length() - file_data_pos - boundary_value.length());


        // Save the file to disk
        std::ofstream outfile("uploads/" + filename_value, std::ios::binary);
        outfile.write(file_data.c_str(), file_data.length());
        outfile.close();
    }
}

