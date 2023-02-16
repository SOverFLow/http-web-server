#include "Response.hpp"
#include <unordered_map>
#include <string>
#include <vector>


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


std::vector<std::string> parse_cookie_header(std::string header) {
    std::vector<std::string> cookies;
    size_t pos = header.find("Cookie: ");
    if (pos != std::string::npos) {
        pos += 8; // skip "Cookie: "
        size_t end_pos = header.find("\r\n", pos);
        std::string cookie_str = header.substr(pos, end_pos - pos);

        while (!cookie_str.empty()) {
            size_t delimiter_pos = cookie_str.find("=");
            if (delimiter_pos == std::string::npos)
                break;

            std::string key = cookie_str.substr(0, delimiter_pos);
            cookie_str.erase(0, delimiter_pos + 1);

            size_t end_pos = cookie_str.find("; ");
            if (end_pos == std::string::npos) {
                end_pos = cookie_str.length();
            }

            std::string value = cookie_str.substr(0, end_pos);
            cookies.push_back(key + "=" + value);

            if (end_pos < cookie_str.length() - 1) {
                cookie_str.erase(0, end_pos + 2);
            } else {
                cookie_str.clear();
            }
        }
    }
    return cookies;
}







void check_is_valid_session_cookie()
{
    bool has_valid_session_cookie = false;
    std::string session_id;
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
}

void generete_new_session_cookie()
{
    // if (!has_valid_session_cookie) {
    //     session_id = generate_session_id();
    //     user_session = Session{};
    // }
}


void add_set_cookie_header()
{
    // Cookie session_cookie;
    // session_cookie.name = "session_id";
    // session_cookie.value = session_id;
    // session_cookie.expiration_time = time(nullptr) + 3600;
    // cookies.push_back(session_cookie);
    // std::string set_cookie_header = generate_set_cookie_header(cookies);
}

std::unordered_map<std::string, Session> sessions;


void   cookie_handler(std::string http_request) {



    std::vector<std::string> cookies = parse_cookie_header(http_request);

    // std::vector<std::string>::iterator it;
    // for (it = cookies.begin(); it != cookies.end(); it++)
    // {
    //     std::cout << *it << std::endl;
    // }

   // check_is_valid_session_cookie();

    // Session user_session;
    // if (has_valid_session_cookie) {
    //     user_session = sessions[session_id];
    // }

   // generete_new_session_cookie();

   //add_set_cookie_header();

}