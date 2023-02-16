#include "Response.hpp"
#include <unordered_map>


class Session
{
    public:
        Session();
        std::string username;
};

class Cookie
{
    public: 
        Cookie();
        std::string name;
        std::string value;
        time_t expiration_time;
};


void parse_cookie_header(const std::string& cookie_header, std::vector<std::string> *cookies) {
    size_t pos = 0;
    while (pos < cookie_header.length()) {
        size_t end_pos = cookie_header.find(';', pos);
        if (end_pos == std::string::npos) {
            end_pos = cookie_header.length();
        }
        std::string cookie = cookie_header.substr(pos, end_pos - pos);
        pos = end_pos + 1;
        size_t eq_pos = cookie.find('=');
        if (eq_pos == std::string::npos) {
            continue;
        }
        std::string key = cookie.substr(0, eq_pos);
        std::string value = cookie.substr(eq_pos + 1);
        std::cout << key << std::endl;
        //cookies.push_back(key + "=" + value);
    }
}

std::unordered_map<std::string, Session> sessions;


void   cookie_handler(std::string http_request) {

    std::vector<Cookie> *cookies;

    //parse_cookie_header(http_request, cookies);
    //std::cout << "cookies: " << std::endl;

}
    // // Check if the client has a valid session cookie
    // bool has_valid_session_cookie = false;
    // std::string session_id;
    // for (const auto& cookie : cookies) {
    //     if (cookie.name == "session_id") {
    //         // Check if the session ID is valid (i.e., if it is in the `sessions` map)
    //         if (sessions.find(cookie.value) != sessions.end()) {
    //             has_valid_session_cookie = true;
    //             session_id = cookie.value;
    //         }
    //         break;
    //     }
    // }

    // // If the client has a valid session cookie, use the session ID to look up their session data
    // Session user_session;
    // if (has_valid_session_cookie) {
    //     user_session = sessions[session_id];
    // }

    // // Generate a new session ID if the client does not have a valid session cookie
    // if (!has_valid_session_cookie) {
    //     session_id = generate_session_id();
    //     user_session = Session{};
    // }

    // Process the incoming HTTP request
    // ...
    // Generate an HTTP response
    // ...

    // Add a Set-Cookie header to the response to set or update the session ID cookie
    // Cookie session_cookie;
    // session_cookie.name = "session_id";
    // session_cookie.value = session_id;
    // session_cookie.expiration_time = time(nullptr) + 3600; // Set the expiration time to 1 hour from now
    // cookies.push_back(session_cookie);
    // std::string set_cookie_header = generate_set_cookie_header(cookies);

    // // Send the HTTP response to the client
    // std::string http_response = generate_http_response(...);
    // http_response += set_cookie_header;
    // // write_http_response(client_sock, http