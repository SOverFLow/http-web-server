#include "Server.hpp"
#include <vector>


void parse_upload_post_data(std::string http_request)
{
    bool http_request_done = false;

    size_t pos = http_request.find("\r\n\r\n");


    if (pos != std::string::npos) 
    {
       std::cout << http_request << std::endl;
        http_request_done = true;


        size_t content_type_pos = http_request.find("Content-Type: multipart/form-data");

        if (content_type_pos != std::string::npos) 
        {
            std::string boundary_str = "boundary=";
            size_t boundary_pos = http_request.find(boundary_str, content_type_pos);
            if (boundary_pos != std::string::npos)
            {
                std::string boundary = http_request.substr(boundary_pos + boundary_str.size(), pos - (boundary_pos + boundary_str.size()));
                
                std::vector<std::string> parts;
                size_t start_pos = pos + 4;
                std::cout << "hada boundry =>"<< boundary << std::endl;
                size_t end_pos = http_request.find("--" + boundary + "--", start_pos);
                std::cout << "hadi end => "<< end_pos << std::endl;
                while (end_pos != std::string::npos) 
                {
                    std::cout << "DKHALT HNA" << std::endl;
                    parts.push_back(http_request.substr(start_pos, end_pos - start_pos));
                    start_pos = end_pos + 2 + boundary.size();
                    end_pos = http_request.find("--" + boundary + "--", start_pos);
                }
            
            for (std::vector<std::string>::const_iterator it = parts.begin(); it != parts.end(); ++it) 
            {
                const std::string& part = *it;
                size_t name_pos = part.find("name=\"");
                if (name_pos == std::string::npos)
                    continue;

                size_t name_start = name_pos + 6;
                size_t name_end = part.find("\"", name_start);
                if (name_end == std::string::npos)
                    continue;
                std::string name = part.substr(name_start, name_end - name_start);
                //std::cout << name << std::endl;

              
                size_t filename_pos = part.find("filename=\"");
                if (filename_pos == std::string::npos)
                    continue;
                size_t filename_start = filename_pos + 10;
                size_t filename_end = part.find("\"", filename_start);
                if (filename_end == std::string::npos)
                    continue;
                std::string filename = part.substr(filename_start, filename_end - filename_start);
                //std::cout << filename << std::endl;


                size_t content_type_pos = part.find("Content-Type: ");
                if (content_type_pos == std::string::npos)
                    continue;
                size_t content_type_start = content_type_pos + 14;
                size_t content_type_end = part.find("\r\n", content_type_start);
                if (content_type_end == std::string::npos)
                    continue;

                std::string content_type = part.substr(content_type_start, content_type_end - content_type_start);

              
                size_t data_start = part.find("\r\n\r\n") + 4;

                size_t data_end = part.size() - 2;
                std::string allPath= "uploads/" + filename;
    
                int file_fd = open(allPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
                if (file_fd == -1) {
                    std::cout << filename << " Error opening file" << std::endl;
                    return ;
                }
                else
                    std::cout << filename <<" <File uploaded>" << std::endl;
                write(file_fd, part.data() + data_start, data_end - data_start);
                close(file_fd);
            }
        }
    }
}
}