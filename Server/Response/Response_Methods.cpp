#include "Response.hpp"

std::string	Response::handel_delete_request(std::string file_path)
{

    std::string res;
    std::string file_content;

	if (CheckIsFile(file_path.substr(1)))
	{
		if (remove(file_path.substr(1).c_str()) == 0)
        {
            res = "HTTP/1.1 204 No Content\r\nContent-type: text/html\r\n\r\n";

        }
		else
        {
            res = "HTTP/1.1 403 Forbidden\r\nContent-type: text/html\r\n\r\n";
            file_content = read_file_content("/Error_Pages/403.html");
            res += file_content;
        }
	}
	else
    {
        res = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\n\r\n";
        file_content = read_file_content("/Error_Pages/404.html");
        res += file_content;
    }
    return (res);
}


std::string Response::handle_get_request(std::string Path, std::string contentType)
{
    std::ifstream file(Path.substr(1), std::ios::binary);
    std::string file_content;
    std::string res;


    if (file)
    {
        if (Path.find(".", 0) == std::string::npos)
        {
            serve_index(Path, contentType);
            return (res);
        }
        else
        {
            serve_other_files(Path, contentType);   
            return (res);
        }
    }
    else if (Path == "/")
    {
        serve_root_path(Path, contentType);
        return (res);
    }
    else
    {
        res = check_request_path(Path) + "text/html" + "\r\n\r\n";
        file_content = read_file_content("/Error_Pages/404.html");
        res += file_content;
    }
    return (res);
}