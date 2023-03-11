#include "Server.hpp"

std::map<std::string, std::string> Server::parse_cookies(std::string request) {
    std::map<std::string, std::string> cookies;
    size_t cookie_pos = request.find("Cookie:");
    if (cookie_pos != std::string::npos) {
        size_t end_cookie_pos = request.find("\r\n", cookie_pos);
        std::string cookie_header = request.substr(cookie_pos + 8, end_cookie_pos - cookie_pos - 8);
        size_t name_pos = 0;
        while (name_pos != std::string::npos) {
            size_t equals_pos = cookie_header.find("=", name_pos);
            if (equals_pos != std::string::npos) {
                std::string name = cookie_header.substr(name_pos, equals_pos - name_pos);
                size_t semicolon_pos = cookie_header.find(";", equals_pos);
                if (semicolon_pos == std::string::npos) {
                    semicolon_pos = cookie_header.length();
                }
                std::string value = cookie_header.substr(equals_pos + 1, semicolon_pos - equals_pos - 1);
                cookies[name] = value;
                name_pos = cookie_header.find(";", semicolon_pos);
                if (name_pos != std::string::npos) {
                    name_pos += 2; // skip the "; "
                }
            } else {
                name_pos = std::string::npos;
            }
        }
    }
    return cookies;
}




// Generate a random session ID
std::string generate_session_id() {
    std::ostringstream session_id_stream;
    session_id_stream << time(NULL) << "_" << rand();
    return session_id_stream.str();
}






std::string make_set_cookie_header(std::string name, std::string value) {
    std::ostringstream header_stream;
    std::string path = "/";
    int max_age = -1;
    header_stream << "Set-Cookie: " << name << "=" << value << "; Path=" << path;
    if (max_age >= 0) {
        header_stream << "; Max-Age=" << max_age;
    }
    header_stream << "\r\n";
    return header_stream.str();
}





std::string Server::manage_cookies_session_server()
{
    // Get session ID from cookies
    std::string session_id;
    std::map<std::string, std::string>::iterator it = cookies.find("session_id");
    if (it != cookies.end()) {
        session_id = it->second;
    }

    // Create new session if no session ID found
    if (session_id.empty()) {
        session_id = generate_session_id();
    }

    // Update session data
    it = sessions.find(session_id);
    if (it == sessions.end()) {
        sessions[session_id] = "session data for " + session_id;
    }

    // Create Set-Cookie header
    std::string set_cookie_header = make_set_cookie_header("session_id", session_id);
    
    return (set_cookie_header);
}