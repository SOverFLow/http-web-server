#ifndef WEBSERVER_REQUEST_HPP
#define WEBSERVER_REQUEST_HPP


#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

class Server;

enum Method {GET, POST, DELETE, NONE};

struct Header {
	std::string	name;
	std::string value;
};

struct Request {
	Method				method;
	std::string			path;
	std::string			version;
	std::vector<Header>	headers;
	std::string			body;
	bool				ready;
};

struct client_info {
	socklen_t				address_len;
	struct sockaddr_storage	address;
	int						sock;
	std::string				request_str;
	Request					request;
};


void	handle_requests(std::vector<Server>& servers);
void	parse_requests(std::vector<client_info>& clients);
Request	get_headers(std::vector<std::string> req);


#endif 