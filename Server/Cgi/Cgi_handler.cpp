#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../Request/Request.hpp"
#include "../../Config/Config.hpp"
#include <vector>
#include <fstream>

char    **setEnv(Request &req, std::string Path, Locations &Location, ServerBlock &Server)
{
    std::vector <char *> env;
    std::string method = req.Method;
    if (req.Method == "GET")
    {
        if(req.Qurey_String != "NULL")
            env.push_back(strdup(("QUERY_STRING=" + req.Qurey_String).c_str()));
    }
    (void)Path;
    env.push_back((char *)("REQUEST_METHOD=" + method).c_str());
    env.push_back(strdup(("HTTP_HOST=" + req.Host).c_str()));
    env.push_back(strdup(("HTTPS=off")));
    env.push_back(strdup(("SCRIPT_NAME=" + Path).c_str()));
    env.push_back(strdup(("DOCUMENT_ROOT=" + Server.root).c_str()));
    env.push_back(NULL);
    return (env.data());
}

std::string get_cgi_output(std::string path, Request &req, Locations &Location, ServerBlock &Server)
{
    std::string res;
    int fd_req[2];
    char c;
    char **env;
    pid_t pid;
    pipe(fd_req);
    if ((pid = fork()) < 0)
        std::cerr << "fork error!" << std::endl;
    if (!pid)
    {
        std::vector<char *> cmds;
        cmds.push_back((char *)"/cgi-bin/php-cgi");
        cmds.push_back(((char *)path.substr(1).data()));
        cmds.push_back(NULL);
        env = setEnv(req, path, Location, Server);
        if (req.Method == "POST")
        {
            dup2(0, 1);
            std::cout << req.Body;
        }
        close(fd_req[0]);
        dup2(fd_req[1], 1);
        if(execve("./cgi-bin/php-cgi", cmds.data(), env) < 0)
        {
            std::cerr << "Exec Error!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    close(fd_req[1]);
    waitpid(pid, NULL, 0);
    while(read(fd_req[0], &c, 1))
        res += c;
    return res;
}

std::string getCtype(std::string Output)
{
    int Ctype_p;
    std::string Content_type;
    Ctype_p = Output.find("Content-type:", 0);
    int end = Ctype_p+14;
    end = Output.find(";", Ctype_p+14);
    Content_type = Output.substr(Ctype_p+14, end - (Ctype_p+14));
    return (Content_type);
}

std::string getBody(std::string Output)
{
    size_t  Body_p;
    std::string Body;
    
    Body_p = Output.find("\r\n\r\n", 0);
    Body = Output.substr(Body_p+4, Output.length() - Body_p+4);
    return(Body);
}

std::string Header_gen( std::string Output, Request &req)
{
    std::string Header;
    std::string Content_type;
    std::string Status;
    std::string Comment;
    if (Output.find("Status:", 0) == std::string::npos)
    {
        Status = "200";
        req.cgiStatus = 200;
        Content_type = getCtype(Output);
        Header = "HTTP/1.1 " + Status + " OK\r\nContent-type:" + Content_type + "\r\n\r\n" + getBody(Output);
    }
    else
    {
        Status = Output.substr(8, 3);
        Content_type = getCtype(Output);
        req.cgiStatus = atoi(Status.c_str());
        for(int i = 12; Output[i] != '\n'; i++)
            Comment += Output[i];
        Header = "HTTP/1.1 " + Status + " " + Comment + "\r\nContent-type:" + Content_type + "\r\n\r\n";
    }
    return (Header);
}


std::string     Cgi_Handler(Request &req, std::string Path, char **env, std::string cgiLang, ServerBlock &Server)
{
    (void)env;
    std::string all;
    std::string out;
    out = get_cgi_output(Path, req, Location, Server);
    all = Header_gen(out, req);
    return all;
}
