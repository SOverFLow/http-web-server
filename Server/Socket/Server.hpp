#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <poll.h>
#include <dirent.h>
#include "../Request/Request.hpp"
#include "../Response/Response.hpp"
#include "../../Config/Config.hpp"



class Server
{
    public:
        Server(Config config);
    private:
        // std::string Method_client;
        // int StatusCode_client;
        // int cgiStatus_client;
        // std::string Content_Type_client;
        // size_t Content_Lenght_client;
        // size_t Body_Size_From_Config_client;
        // std::string Body_client;
        // std::string Host_client;
        // std::string Qurey_String_client;
        // std::string Path_client;
        // bool    is_Cgi_client;
        Request req;


        bool client_first_read;
        int check_upload_status;
        size_t file_bytes_received;
        size_t file_content_length;
        size_t first_read_data_size;
        std::string file_name_upload;
        std::string file_bondary_upload;
        std::string data;
        int max_socket;
        std::string tmp_path;
        std::vector<std::string> tmp_methods;
        std::vector<std::string> tmp_index;
        int path_check;
        std::vector<int> sockets;
        std::vector<std::string> root_paths;
        std::vector<int> setup_sockets(std::vector<ServerBlock> &servers);
        std::map<std::string, std::string> cookies;
        std::map<std::string, std::string> sessions;
        std::map<std::string, std::string> error_pages;
        std::string cookies_part;
        void connection(std::vector<ServerBlock> &servers);
        void handle_client_request(int client_socket);
        int parse_upload_post_data(std::string full_request, std::string upload_path);
        int parse_upload_post_data_part_two(std::string full_request, std::string upload_path);
        void respond_to_clients(int client_socket, std::string root_path, ServerBlock server, int tmp);
        std::string Return_Error_For_Bad_Request(int status);
        std::map<std::string, std::string> parse_cookies(std::string request);
        std::string manage_cookies_session_server();
};

std::string Return_File_Content(std::string Path);
int set_nonblocking(int fd);
std::vector<pollfd> create_pollfds(std::vector<ServerBlock>& servers);
void handle_new_connection(int listening_socket, std::vector<pollfd> &fds);
int check_if_url_is_location(std::string url, std::vector<Locations> locations);
std::string get_root_location(std::string url, std::vector<Locations> locations);
std::vector<std::string> get_index_location(std::string url, std::vector<Locations> locations);
bool Check_is_method_allowed(std::string method, std::vector<std::string> allowed_methods);
std::vector<std::string> get_allowed_methods(std::string url, std::vector<Locations> locations);
std::string get_redirect_url_for_location(std::string url, std::vector<Locations> locations);
bool check_if_location_has_redirect(std::string url, std::vector<Locations> locations);
bool Check_Cgi_Location_Status(std::string url, std::vector<Locations> locations);
std::string get_index_file_name_cgi(std::vector<std::string> index, std::string path);
std::string serve_index_for_cgi(std::string Path, std::vector<std::string> index_files);
int get_redirect_code_for_location(std::string url, std::vector<Locations> locations);
std::string return_redirect_msg(int code);
bool Check_upload_Location_Status(std::string url, std::vector<Locations> locations);
std::string Get_upload_Location_Path(std::string url, std::vector<Locations> locations);
std::string generate_session_id();
std::string make_set_cookie_header(std::string name, std::string value);
Locations get_location(std::string url, std::vector<Locations> locations);

