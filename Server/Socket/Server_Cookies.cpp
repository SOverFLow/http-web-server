#include "Server.hpp"

std::map<std::string, std::string> Server::parse_cookies(std::string request) {
    std::map<std::string, std::string> cookies;

    size_t cookie_pos = request.find("Cookie:");

    if (cookie_pos != std::string::npos) 
    {
        size_t end_cookie_pos = request.find("\r\n", cookie_pos);

        std::string cookie_header = request.substr(cookie_pos + 8, end_cookie_pos - cookie_pos - 8);

        size_t name_pos = 0;

        while (name_pos != std::string::npos) 
        {
            size_t equals_pos = cookie_header.find("=", name_pos);
            if (equals_pos != std::string::npos) 
            {
                std::string name = cookie_header.substr(name_pos, equals_pos - name_pos);

                size_t semicolon_pos = cookie_header.find(";", equals_pos);

                if (semicolon_pos == std::string::npos) 
                {
                    semicolon_pos = cookie_header.length();
                }

                std::string value = cookie_header.substr(equals_pos + 1, semicolon_pos - equals_pos - 1);

                cookies[name] = value;

                name_pos = cookie_header.find(";", semicolon_pos);
                
                if (name_pos != std::string::npos)
                {
                    name_pos += 2;
                }
            } 
            else 
            {
                name_pos = std::string::npos;
            }
        }
    }
    return cookies;
}


std::string generate_session_id() {
    std::ostringstream session_id_stream;
    session_id_stream << time(NULL) << "_" << rand();
    return session_id_stream.str();
}


std::string make_set_cookie_header(std::string name, std::string value) {
    std::ostringstream header_stream;
    header_stream << "Set-Cookie: " << name << "=" << value << ";";
    header_stream << "\r\n";
    return header_stream.str();
}


std::string Server::manage_cookies_session_server()
{
    std::string session_id;
    std::map<std::string, std::string>::iterator it = cookies.find("session_id");
    if (it != cookies.end())
        session_id = it->second;

    if (session_id.empty())
        session_id = generate_session_id();

    it = sessions.find(session_id);
    if (it == sessions.end())
        sessions[session_id] = "session data for " + session_id;

    std::string set_cookie_header = make_set_cookie_header("session_id", session_id);
    
    return (set_cookie_header);
}